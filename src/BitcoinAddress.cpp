// Copyright (c) 2014-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "BitcoinAddress.h"

namespace
{
    class CBitcoinAddressVisitor : public boost::static_visitor<bool>
    {
    private:
        CBitcoinAddress* addr;

    public:
        CBitcoinAddressVisitor(CBitcoinAddress* addrIn) : addr(addrIn) {}

        bool operator()(const CKeyID& id) const { return addr->Set(id); }
        bool operator()(const CScriptID& id) const { return addr->Set(id); }
        bool operator()(const CNoDestination& no) const { return false; }
    };

} // anon namespace

bool CBitcoinAddress::Set(const CKeyID& id)
{
    SetData(Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS), &id, 20);
    return true;
}

bool CBitcoinAddress::Set(const CScriptID& id)
{
    SetData(Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS), &id, 20);
    return true;
}

bool CBitcoinAddress::Set(const CTxDestination& dest)
{
    return boost::apply_visitor(CBitcoinAddressVisitor(this), dest);
}

bool CBitcoinAddress::IsValid() const
{
    return IsValid(Params());
}

bool CBitcoinAddress::IsValid(const CChainParams& params) const
{
    bool fCorrectSize = vchData.size() == 20;
    bool fKnownVersion = vchVersion == params.Base58Prefix(CChainParams::PUBKEY_ADDRESS) ||
        vchVersion == params.Base58Prefix(CChainParams::SCRIPT_ADDRESS);
    return fCorrectSize && fKnownVersion;
}

CTxDestination CBitcoinAddress::Get() const
{
    if (!IsValid())
        return CNoDestination();
    uint160 id;
    memcpy(&id, &vchData[0], 20);
    if (vchVersion == Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS))
        return CKeyID(id);
    else if (vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS))
        return CScriptID(id);
    else
        return CNoDestination();
}

bool CBitcoinAddress::GetKeyID(CKeyID& keyID) const
{
    if (!IsValid() || vchVersion != Params().Base58Prefix(CChainParams::PUBKEY_ADDRESS))
        return false;
    uint160 id;
    memcpy(&id, &vchData[0], 20);
    keyID = CKeyID(id);
    return true;
}

bool CBitcoinAddress::IsScript() const
{
    return IsValid() && vchVersion == Params().Base58Prefix(CChainParams::SCRIPT_ADDRESS);
}