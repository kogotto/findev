#include <OrderCache.h>

void OrderCache::addOrder(Order order) {
    m_orders.insert({order.orderId(), order});
    m_userToOrderId[order.user()].insert(order.orderId());
    m_secIdToOrderId[order.securityId()].insert(order.orderId());
}

void OrderCache::cancelOrder(const std::string& orderId) {
    const auto it = m_orders.find(orderId);
    if (it == m_orders.end()) {
        return;
    }
    const Order& order = it->second;
    eraseFromUserToOrderId(order.user(), orderId);
    eraseFromSecIdToOrderId(order.securityId(), orderId);
    m_orders.erase(it);
}

void OrderCache::cancelOrdersForUser(const std::string& user) {
    const auto it = m_userToOrderId.find(user);
    if (it == m_userToOrderId.end()) {
        return;
    }

    auto& orderIds = it->second;
    for (const auto& orderId : orderIds) {
        const auto orderIt = m_orders.find(orderId);
        const Order& order = orderIt->second;
        eraseFromSecIdToOrderId(order.securityId(), orderId);
        m_orders.erase(orderIt);
    }
    m_userToOrderId.erase(it);
}

void OrderCache::cancelOrdersForSecIdWithMinimumQty(
        const std::string& securityId,
        unsigned int minQty) {
    const auto ordersForSecIdIt = m_secIdToOrderId.find(securityId);
    if (ordersForSecIdIt == m_secIdToOrderId.end()) {
        return;
    }

    auto& orderIds = ordersForSecIdIt->second;
    const auto ordersIdsEnd = orderIds.end();
    for (auto it = orderIds.begin(); it != ordersIdsEnd;) {
        const std::string& orderId = *it;
        const Order& order = m_orders.find(orderId)->second;
        if (order.qty() >= minQty) {
            eraseFromUserToOrderId(order.user(), orderId);
            m_orders.erase(orderId);
            it = orderIds.erase(it);
        }
        else {
            ++it;
        }
    }
    if (orderIds.empty()) {
        m_secIdToOrderId.erase(ordersForSecIdIt);
    }
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

void OrderCache::eraseFromUserToOrderId(
        const std::string& user,
        const std::string& orderId) {
    const auto it = m_userToOrderId.find(user);
    auto& orderIds = it->second;
    orderIds.erase(orderId);
    if (orderIds.empty()) {
        m_userToOrderId.erase(it);
    }
}

void OrderCache::eraseFromSecIdToOrderId(
        const std::string& securityId,
        const std::string& orderId) {
    const auto it = m_secIdToOrderId.find(securityId);
    auto& orderIds = it->second;
    orderIds.erase(orderId);
    if (orderIds.empty()) {
        m_secIdToOrderId.erase(it);
    }
}
