// x----------------------x
// | safe_struct_writer.cc |
// x----------------------x

#include <fstream>
#include <vector>
#include <iostream>

#include "di/Account.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Expected exactly 3 args\n";
        std::exit(EXIT_FAILURE);
    }

    std::ifstream fin(argv[1]);
    if (!fin)
    {
        std::cerr << "Can't open file: " << strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::ofstream fout(argv[2], std::ios::binary);
    if (!fout)
    {
        std::cerr << "Can't open write file: " << strerror(errno) << '\n';
        std::exit(EXIT_FAILURE);
    }

    constexpr int k_buffer_len{ 100 };
    char buffer[k_buffer_len];
    for (std::string line, sep; getline(fin, line);)
    {
        di::Account account{buffer, k_buffer_len};
        int i{};
        for (std::istringstream in{line}; getline(in, sep, ',');++i)
        {
            switch (i)
            {
            case 0:
                account.sequencingId(atoll(sep.c_str()));
                break;
            case 1:
                account.accountId(atoll(sep.c_str()));
                break;
            case 2:
                account.participant().participantId(atoll(sep.c_str()));
                break;
            case 3:
                {
                    di::KycLevelType::Value type{ di::KycLevelType::invalid };
                    if (sep == "id_verified")
                    {
                        type = di::KycLevelType::idVerified;
                    }
                    else if (sep == "address_verified")
                    {
                        type = di::KycLevelType::addressVerified;
                    }
                    account.participant().kycLevel().kycLevelType( type );
                }
                break;
            case 4:
                {
                    di::AccountStatusType::Value type{ di::AccountStatusType::invalid };
                    if (sep == "active")
                    {
                        type = di::AccountStatusType::active;
                    }
                    else if (sep == "inactive")
                    {
                        type = di::AccountStatusType::inactive;
                    }
                    account.accountStatus().accountStatusType( type );
                }
                break;
            case 5:
                {
                    std::string sep_tag;
                    std::vector<std::string> vals;
                    for (std::istringstream in(sep); std::getline(in, sep_tag, ';');)
                    {
                        vals.emplace_back(sep_tag);
                    }
                    di::Account::Tags& tags = account.tagsCount(static_cast<uint16_t>(vals.size()));
                    for (auto val: vals)
                    {
                        tags.next().putTag(std::move(val));
                    }
                }
                break;
            case 6:
                {
                    di::SelfTradingRuleType::Value type{ di::SelfTradingRuleType::invalid };
                    if (sep == "valid")
                    {
                        type = di::SelfTradingRuleType::valid;
                    }
                    account.selfTradingRule().selfTradingRuleType( type );
                }
                break;
            case 7:
                {
                    di::CancellationPolicyType::Value type{ di::CancellationPolicyType::invalid };
                    if (sep == "valid")
                    {
                        type = di::CancellationPolicyType::valid;
                    }
                    account.cancellationPolicy().cancellationPolicyType( type );
                }
                break;
            default:
                std::cerr << "Incorrect record: " << line << '\n';
                std::exit(EXIT_FAILURE);
            }
        }
        fout.write(buffer, k_buffer_len);
    }
    return EXIT_SUCCESS;
}
