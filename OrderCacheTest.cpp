#include <gtest/gtest.h>

#include <set>

#include <OrderCache.h>

namespace {

using Orders = std::vector<Order>;

std::set<Order> toSet(const Orders& orders) {
    return std::set<Order>(orders.begin(), orders.end());
}

bool sameOrders(const Orders& lhs, const Orders& rhs) {
    return toSet(lhs) == toSet(rhs);
}

TEST(OrderCache, foo) {
    ASSERT_EQ(foo(), 666);
}

TEST(OrderCache, addOrder) {
    OrderCache cache;
    ASSERT_EQ(cache.getAllOrders().size(), 0);

    const Order order1{"order1", "secId1", "Buy", 1000, "user1", "company1"};
    cache.addOrder(order1);
    const Orders actual1 = cache.getAllOrders();
    const Orders expected1{order1};
    ASSERT_TRUE(sameOrders(actual1, expected1));

    const Order order2{"order2", "secId2", "Buy", 2000, "user2", "company2"};
    cache.addOrder(order2);
    const Orders actual2 = cache.getAllOrders();
    const Orders expected2{order1, order2};
    ASSERT_TRUE(sameOrders(actual2, expected2));
}

} // namespace
