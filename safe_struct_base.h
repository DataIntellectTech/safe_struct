//x--------------------x
//| safe_struct_base.h |
//x--------------------x
#ifndef SAFE_STRUCT_BASE_H_
#define SAFE_STRUCT_BASE_H_

#include "fake_boost.h"

#include <limits>
#include <optional>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>

#define ENUM_LLINT(x) enum class x: long long { invalid = std::numeric_limits<long long>::min() }
#define ENUM_CLASS(x, ...) enum class x{ invalid, __VA_ARGS__ }

namespace core_types
{

using fake_boost::flat_set;
constexpr int k_tag_len{ 20 };
typedef char tag[k_tag_len];

}

namespace session
{

ENUM_LLINT(sequencing_id);
ENUM_LLINT(source_id);
ENUM_LLINT(destination_id);

}

namespace account_management
{

ENUM_LLINT(account_id);
ENUM_CLASS(account_status, active, inactive);

}

namespace party_data
{

ENUM_LLINT(participant_id);
ENUM_CLASS(kyc_level, id_verified, address_verified);

}

namespace trading
{

ENUM_CLASS(self_trading_rule);
ENUM_CLASS(cancellation_policy, valid);

}

struct RetailParticipant
{
party_data::participant_id participant_id{ party_data::participant_id::invalid };
party_data::kyc_level kyc_level{ party_data::kyc_level::invalid };
constexpr bool operator<=>(const RetailParticipant&) const = default;
};

template <typename ParticipantT>
struct Account
{
session::sequencing_id sequencing_id{ session::sequencing_id::invalid };
account_management::account_id account_id{ account_management::account_id::invalid };
ParticipantT participant{};
account_management::account_status account_status{ account_management::account_status::invalid };
core_types::flat_set<core_types::tag> tags{};
std::optional<trading::self_trading_rule> self_trading_rule;
std::optional<trading::cancellation_policy> cancellation_policy;
constexpr bool operator<=>(const Account<ParticipantT>&) const = default;

std::string ToString()
{
    std::stringstream ss;
    using llong = long  long;
    for (long long t: std::vector<llong>{(llong)sequencing_id,
                                         (llong)account_id,
                                         (llong)participant.participant_id,
                                         (llong)participant.kyc_level,
                                         (llong)account_status})
    {
        ss << t << ',';
    }
    for (int i{}; i<tags.Size(); ++i)
    {
        if (strlen(tags.m_arr_[i]))
        {
            ss << "[" << tags.m_arr_[i] << "]";
        }
    }
    if (self_trading_rule)
    {
        ss << (llong)(*self_trading_rule) << ',';
    }
    if (cancellation_policy)
    {
        ss << (llong)(*cancellation_policy) << ',';
    }
    return ss.str();
}
};

constexpr bool IsInvalid(const RetailParticipant& participant) noexcept
{
    constexpr auto k_invalid{ RetailParticipant{} };
    return participant.participant_id == k_invalid.participant_id ||
            participant.kyc_level == k_invalid.kyc_level;
}

template <typename ParticipantT>
constexpr bool IsInvalid(const Account<ParticipantT>& account) noexcept
{
    constexpr auto k_invalid{ Account<ParticipantT>{} };
    return account.account_id == k_invalid.account_id ||
            account.account_status == k_invalid.account_status ||
            account.cancellation_policy == k_invalid.cancellation_policy ||
            IsInvalid(account.participant) ||
            account.self_trading_rule == k_invalid.self_trading_rule ||
            account.sequencing_id == k_invalid.sequencing_id ||
            account.tags == k_invalid.tags;
}

template <typename StructT>
auto CreateAccountValid(const StructT& struct_in)
{
    return IsInvalid(struct_in)? std::optional<StructT>{}: std::optional<StructT>{ struct_in };
}

Account<RetailParticipant> CreateAccount(   session::sequencing_id sid,
                                            account_management::account_id aid,
                                            party_data::participant_id pid,
                                            account_management::account_status status,
                                            party_data::kyc_level kyc,
                                            core_types::flat_set<core_types::tag>&& tags,
                                            trading::self_trading_rule str,
                                            trading::cancellation_policy cp)
{
    Account<RetailParticipant> account{};
    account.sequencing_id = sid;
    account.account_id = aid;
    account.participant.participant_id = pid;
    account.account_status = status;
    account.participant.kyc_level = kyc;
    account.tags = std::move(tags);
    account.self_trading_rule = str;
    account.cancellation_policy = cp;
    return account;
}

#endif
