// x------------------------x
// | safe_struct_reader.cc |
// x------------------------x
#include "safe_struct_base.h"
#include "fake_boost.h"
#include "di/Account.h"

#include <chrono>
#include <iostream>
#include <fstream>

#define RUN_TIMED_TEST  fin.clear(); \
                        fin.seekg(0);\
                        auto start_time{ std::chrono::system_clock::now() }

#define FINISH_TIMED_TEST   auto now{ std::chrono::system_clock::now() }; \
                            std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(now-start_time).count() << "ns\n"

#define GET_ACCOUNT_TAGS    di::Account di_account{ buffer, 100 }; \
                            core_types::flat_set<core_types::tag> tags{}; \
                            int i{}; \
                            di_account.tags().forEach([&](di::Account::Tags& in_tag) { \
                                in_tag.getTag(tags.m_arr_[i++], in_tag.tagLength());   \
                            });                                                        \
                            tags.m_size_ = i

//#define WARMUP_IO_CACHE    std::ifstream fin_cache(argv[2]); char buff_cache[100]; while (fin_cache.read(buff_cache, 100))

void Usage()
{
    std::cerr << "safe_struct_reader <sbe_output> <all|safe_struct|conv_struct|direct>\n";
    std::exit(EXIT_FAILURE);
}

void TestSafeStruct(std::ifstream& fin, int bytes_read)
{
    char buffer[bytes_read];
    while (fin.read(buffer, bytes_read))
    {
        GET_ACCOUNT_TAGS;
        CreateAccountValid( Account<RetailParticipant>{
            .sequencing_id{ session::sequencing_id(di_account.sequencingId()) },
            .account_id{ account_management::account_id(di_account.accountId()) },
            .participant {
                .participant_id{ party_data::participant_id{ di_account.participant().participantId() }},
                .kyc_level{ party_data::kyc_level{ di_account.participant().kycLevel().kycLevelType()} }
            },
            .account_status{ account_management::account_status{ di_account.accountStatus().accountStatusType() }},
            .tags{ core_types::flat_set<core_types::tag>{ std::move(tags) }},
            .self_trading_rule = trading::self_trading_rule(di_account.selfTradingRule().selfTradingRuleType()),
            .cancellation_policy{ trading::cancellation_policy{ di_account.cancellationPolicy().cancellationPolicyType() }}
        });
    }
}

void TestConvStruct(std::ifstream& fin, int bytes_read)
{
    char buffer[bytes_read];
    while (fin.read(buffer, bytes_read))
    {
        GET_ACCOUNT_TAGS;
        CreateAccount(  session::sequencing_id(di_account.sequencingId()),
                        account_management::account_id(di_account.accountId()),
                        party_data::participant_id(di_account.participant().participantId()),
                        account_management::account_status(di_account.accountStatus().accountStatusType()),
                        party_data::kyc_level(di_account.participant().kycLevel().kycLevelType()),
                        std::move(tags),
                        trading::self_trading_rule(di_account.selfTradingRule().selfTradingRuleType()),
                        trading::cancellation_policy(di_account.cancellationPolicy().cancellationPolicyType()));
    }
}

void TestDesignateStruct(std::ifstream& fin, int bytes_read)
{
    char buffer[bytes_read];
    while (fin.read(buffer, bytes_read))
    {
        GET_ACCOUNT_TAGS;
        Account<RetailParticipant>{
            .sequencing_id = session::sequencing_id(di_account.sequencingId()),
            .account_id = account_management::account_id(di_account.accountId()),
            .participant = {
                .participant_id = party_data::participant_id(di_account.participant().participantId()),
                .kyc_level = party_data::kyc_level(di_account.participant().kycLevel().kycLevelType())
            },
            .account_status = account_management::account_status(di_account.accountStatus().accountStatusType()),
            .tags = std::move(tags),
            .self_trading_rule = trading::self_trading_rule(di_account.selfTradingRule().selfTradingRuleType()),
            .cancellation_policy = trading::cancellation_policy(di_account.cancellationPolicy().cancellationPolicyType())
        };
    }
}

void TestDirectStruct(std::ifstream& fin, int bytes_read)
{
    char buffer[bytes_read];
    while (fin.read(buffer, bytes_read))
    {
        GET_ACCOUNT_TAGS;
        Account<RetailParticipant> created_account{};
        created_account.sequencing_id = session::sequencing_id{di_account.sequencingId()};
        created_account.account_id = account_management::account_id{di_account.accountId()};
        created_account.participant = {
            .participant_id = party_data::participant_id{di_account.participant().participantId()},
            .kyc_level = party_data::kyc_level(di_account.participant().kycLevel().kycLevelType())
        };
        created_account.account_status = account_management::account_status{di_account.accountStatus().accountStatusType()};
        created_account.tags = std::move(tags);
        created_account.self_trading_rule = trading::self_trading_rule{di_account.selfTradingRule().selfTradingRuleType()};
        created_account.cancellation_policy = trading::cancellation_policy{di_account.cancellationPolicy().cancellationPolicyType()};
    }
}

int main(int argc, char* argv[])
{
    constexpr int k_safe_struct_indx{ 0 };
    constexpr int k_conv_struct_indx{ 1 };
    constexpr int k_delegate_indx{ 2 };
    constexpr int k_direct_indx{ 3 };
    constexpr fake_boost::flat_set<char[15], 4> avail_tests {
        .m_arr_{{"safe_struct"}, {"conv_struct"}, {"designate"}, {"direct"}},
        .m_size_{ 4 }
    };
    constexpr int buffer_size{ 100 };
    if (argc != 3)
    {
        Usage();
    }

    std::ifstream fin(argv[1]);
    if (!fin)
    {
        std::cerr << "Can't open ifstream: " << strerror(errno) << "\n";
        std::exit(EXIT_FAILURE);
    }

    bool is_all_test{ false };
    bool is_any_tested{ false };
    if (strcmp(argv[2], "all") == 0)
    {
        is_all_test = true;
        is_any_tested = true;
    }

    if (is_all_test || strcmp(argv[2], avail_tests.m_arr_[k_safe_struct_indx]) == 0)
    {
        is_any_tested = true;
        std::cout << "safe_struct runtime: ";
        RUN_TIMED_TEST;
        //WARMUP_IO_CACHE;
        TestSafeStruct(fin, buffer_size);
        FINISH_TIMED_TEST;
    }

    if (is_all_test || strcmp(argv[2], avail_tests.m_arr_[k_direct_indx]) == 0)
    {
        is_any_tested = true;
        std::cout << "direct struct runtime: ";
        RUN_TIMED_TEST;
        //WARMUP_IO_CACHE;
        TestDirectStruct(fin, buffer_size);
        FINISH_TIMED_TEST;
    }

    if (is_all_test || strcmp(argv[2], avail_tests.m_arr_[k_conv_struct_indx]) == 0)
    {
        is_any_tested = true;
        std::cout << "conv_struct runtime: ";
        RUN_TIMED_TEST;
        //WARMUP_IO_CACHE;
        TestConvStruct(fin, buffer_size);
        FINISH_TIMED_TEST;
    }

    if (is_all_test || strcmp(argv[2], avail_tests.m_arr_[k_delegate_indx]) == 0)
    {
        is_any_tested = true;
        std::cout << "designate struct rutime: ";
        RUN_TIMED_TEST;
        //WARMUP_IO_CACHE;
        TestDesignateStruct(fin, buffer_size);
        FINISH_TIMED_TEST;
    }

    if (!is_any_tested)
    {
        std::cerr << "Unrecognized option: " << argv[2] << "; available options: ";
        for (int i{ 0 }; i<avail_tests.Size(); ++i)
        {
            std::cerr << avail_tests.m_arr_[i] << ", ";
        }
        std::cerr << '\n';
        std::exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}



