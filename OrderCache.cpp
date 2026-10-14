#include <OrderCache.h>

#include <algorithm>
#include <cstdint>

namespace {

struct CompanyQty {
    uint64_t buy{0};
    uint64_t sell{0};
};
using CompaniesQty = std::unordered_map<std::string, CompanyQty>;

uint64_t getMaxCompanyQty(const CompaniesQty& companiesQty) {
    uint64_t max = 0;
    for (const auto& [_, companyQty] : companiesQty) {
        const uint64_t qty = companyQty.buy + companyQty.sell;
        if (max < qty) {
            max = qty;
        }
    }
    return max;
}

const std::string BUY_SIDE = "Buy";

} // namespace

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
    const auto ordersForSecIdIt = m_secIdToOrderId.find(securityId);
    if (ordersForSecIdIt == m_secIdToOrderId.end()) {
        return 0;
    }

    CompaniesQty companiesQty;
    uint64_t totalBuy = 0;
    uint64_t totalSell = 0;

    for (const auto& orderId : ordersForSecIdIt->second) {
        const Order& order = m_orders.find(orderId)->second;
        auto& company = companiesQty[order.company()];
        if (order.side() == BUY_SIDE) {
            company.buy += order.qty();
            totalBuy += order.qty();
        }
        else {
            company.sell += order.qty();
            totalSell += order.qty();
        }
    }

    const uint64_t maxCompanyQty = getMaxCompanyQty(companiesQty);
    const uint64_t result = std::min({
        totalBuy,
        totalSell,
        totalBuy + totalSell - maxCompanyQty
    });

    return static_cast<unsigned int>(result);
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
