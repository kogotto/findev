#pragma once

#include <unordered_map>
#include <unordered_set>

#include <OrderCacheInterface.hpp>

class OrderCache : public OrderCacheInterface {
public:
    void addOrder(Order order) override;
    void cancelOrder(const std::string& orderId) override;
    void cancelOrdersForUser(const std::string& user) override;
    void cancelOrdersForSecIdWithMinimumQty(
            const std::string& securityId,
            unsigned int minQty) override;
    unsigned int getMatchingSizeForSecurity(
            const std::string& securityId) override;
    std::vector<Order> getAllOrders() const override;
private:
    void eraseFromUserToOrderId(
            const std::string& user,
            const std::string& orderId);
    void eraseFromSecIdToOrderId(
            const std::string& securityId,
            const std::string& orderId);
    // Main storage. Key - orderId
    std::unordered_map<std::string, Order> m_orders;
    // Key - user, value - set of orderId
    std::unordered_map<std::string, std::unordered_set<std::string>>
        m_userToOrderId;
    // Key - securityId, value - set of orderId
    std::unordered_map<std::string, std::unordered_set<std::string>>
        m_secIdToOrderId;
};
