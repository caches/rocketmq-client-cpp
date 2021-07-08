#include "rocketmq/DefaultMQProducer.h"
#include "DefaultMQProducerImpl.h"
#include "MixAll.h"

#include "absl/strings/str_split.h"

ROCKETMQ_NAMESPACE_BEGIN

DefaultMQProducer::DefaultMQProducer(const std::string& group_name)
    : impl_(std::make_shared<DefaultMQProducerImpl>(group_name)) {}

void DefaultMQProducer::start() { impl_->start(); }

void DefaultMQProducer::shutdown() { impl_->shutdown(); }

int DefaultMQProducer::getSendMsgTimeout() const {
  return static_cast<int>(absl::ToInt64Milliseconds(impl_->getIoTimeout()));
}

void DefaultMQProducer::setSendMsgTimeout(int timeout_millis) {
  impl_->setIoTimeout(absl::Milliseconds(timeout_millis));
}

void DefaultMQProducer::setNamesrvAddr(const std::string& name_server_address_list) {
  std::vector<std::string> name_server_list = absl::StrSplit(name_server_address_list, ';');
  impl_->setNameServerList(name_server_list);
}

void DefaultMQProducer::setGroupName(const std::string& group_name) { impl_->setGroupName(group_name); }

void DefaultMQProducer::setInstanceName(const std::string& instance_name) { impl_->setInstanceName(instance_name); }

void DefaultMQProducer::enableTracing(bool enabled) { impl_->enableTracing(enabled); }

bool DefaultMQProducer::isTracingEnabled() { return impl_->isTracingEnabled(); }

SendResult DefaultMQProducer::send(const rocketmq::MQMessage& message, bool filter_active_broker) {
  return impl_->send(message);
}

SendResult DefaultMQProducer::send(const MQMessage& msg, const MQMessageQueue& mq) { return impl_->send(msg, mq); }

SendResult DefaultMQProducer::send(const MQMessage& msg, MessageQueueSelector* selector, void* arg) {
  return impl_->send(msg, selector, arg);
}

SendResult DefaultMQProducer::send(const MQMessage& message, MessageQueueSelector* selector, void* arg, int retry_times,
                                   bool select_active_broker) {
  return impl_->send(message, selector, arg, retry_times);
}

void DefaultMQProducer::send(const MQMessage& message, SendCallback* send_callback, bool select_active_broker) {
  impl_->send(message, send_callback);
}

void DefaultMQProducer::send(const MQMessage& message, const MQMessageQueue& message_queue,
                             SendCallback* send_callback) {
  impl_->send(message, message_queue, send_callback);
}

void DefaultMQProducer::send(const MQMessage& message, MessageQueueSelector* selector, void* arg,
                             SendCallback* send_callback) {
  impl_->send(message, selector, arg, send_callback);
}

void DefaultMQProducer::sendOneway(const MQMessage& message, bool select_active_broker) { impl_->sendOneway(message); }

void DefaultMQProducer::sendOneway(const MQMessage& message, const MQMessageQueue& message_queue) {
  impl_->sendOneway(message, message_queue);
}

void DefaultMQProducer::sendOneway(const MQMessage& message, MessageQueueSelector* selector, void* arg) {
  impl_->sendOneway(message, selector, arg);
}

void DefaultMQProducer::setRetryTimes(int retry_times) { impl_->maxAttemptTimes(retry_times); }

int DefaultMQProducer::getRetryTimes() const { return impl_->maxAttemptTimes(); }

std::vector<MQMessageQueue> DefaultMQProducer::getTopicMessageQueueInfo(const std::string& topic) {
  return impl_->getTopicMessageQueueInfo(topic);
}

void DefaultMQProducer::setUnitName(std::string unit_name) { impl_->setUnitName(std::move(unit_name)); }

const std::string& DefaultMQProducer::getUnitName() { return impl_->getUnitName(); }

uint32_t DefaultMQProducer::compressBodyThreshold() const { return impl_->compressBodyThreshold(); }

void DefaultMQProducer::compressBodyThreshold(uint32_t threshold) { impl_->compressBodyThreshold(threshold); }

void DefaultMQProducer::arn(const std::string& arn) { impl_->arn(arn); }

void DefaultMQProducer::setCredentialsProvider(CredentialsProviderPtr credentials_provider) {
  impl_->setCredentialsProvider(std::move(credentials_provider));
}

void DefaultMQProducer::setRegion(const std::string& region) {
  impl_->region(region);
}

ROCKETMQ_NAMESPACE_END