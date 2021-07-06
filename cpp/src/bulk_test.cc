#include <functional>
#include <iostream>
#include <vector>
#include <utility>

#include "ReCalc.hpp"
#include "AggregationFunctions.hpp"
#include "FiBA.hpp"

typedef long long int timestamp;

using namespace std;
int main(int argc, char* argv[]) {

    auto bfinger_agg = btree::make_aggregate<timestamp, 2, btree::finger>(Sum<int>(), 0);

    bfinger_agg.insert(1, 101);
    bfinger_agg.insert(500, 100+500);
    bfinger_agg.insert(1000, 100+1000);
    bfinger_agg.insert(1500, 100+1500);

    vector<pair<timestamp, int>> bulkOne{
        make_pair(5, 105),
        make_pair(507, 100+507),
        make_pair(509, 100+509),
        make_pair(511, 100+511),
        make_pair(515, 100+515),
        make_pair(1700, 100+1700),
        make_pair(1701, 100+1701),
        make_pair(1702, 100+1702),
        make_pair(1703, 100+1703)
    };

    bfinger_agg.bulkInsert(bulkOne);

    vector<pair<timestamp, int>> bulkTwo {
        make_pair(6, 100+6),
        make_pair(7, 100+7),
        make_pair(8, 100+8),
        make_pair(9, 100+9),
        make_pair(1705, 100+1705),
        make_pair(1706, 100+1706),
        make_pair(1707, 100+1707),
        make_pair(1708, 100+1708),
        make_pair(1709, 100+1709),
        make_pair(1710, 100+1710),
        make_pair(1711, 100+1711)
    };
    bfinger_agg.bulkInsert(bulkTwo);

    auto ans = bfinger_agg.query();
    std::cout << "ans = " << ans << std::endl;

    return 0;
}
