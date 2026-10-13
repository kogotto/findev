#include <OrderCache.h>

void OrderCache::addOrder(Order order) {
    m_orders.insert({order.orderId(), order});
    m_userToOrderId[order.user()].insert(order.orderId());
    m_secIdToOrderId[order.securityId()].insert(order.orderId());
}

void OrderCache::cancelOrder(const std::string& orderId) {

}

void OrderCache::cancelOrdersForUser(const std::string& user) {

}

void OrderCache::cancelOrdersForSecIdWithMinimumQty(
        const std::string& securityId,
        unsigned int minQty) {

}

unsigned int OrderCache::getMatchingSizeForSecurity(
        const std::string& securityId) {
    return 0;
}

std::vector<Order> OrderCache::getAllOrders() const {
    std::vector<Order> result;
    result.reserve(m_orders.size());
    for (const auto& [_, order] : m_orders) {
        result.emplace_back(order);
    }
    return result;
}

int foo() {
    return 666;
}
