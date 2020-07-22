/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __DEFAULT_MQ_PRODUCER_IMPL_H__
#define __DEFAULT_MQ_PRODUCER_IMPL_H__

#include "CommunicationMode.h"
#include "DefaultMQProducer.h"
#include "MQClientImpl.h"
#include "MQProducerInner.h"
#include "MessageBatch.h"

namespace rocketmq {

class TopicPublishInfo;
class MQFaultStrategy;
class thread_pool_executor;

class DefaultMQProducerImpl;
typedef std::shared_ptr<DefaultMQProducerImpl> DefaultMQProducerImplPtr;

class DefaultMQProducerImpl : public std::enable_shared_from_this<DefaultMQProducerImpl>,
                              public MQProducer,
                              public MQClientImpl,
                              public MQProducerInner {
 public:
  static DefaultMQProducerImplPtr create(DefaultMQProducerConfigPtr config, RPCHookPtr rpcHook = nullptr) {
    if (nullptr == rpcHook) {
      return DefaultMQProducerImplPtr(new DefaultMQProducerImpl(config));
    } else {
      return DefaultMQProducerImplPtr(new DefaultMQProducerImpl(config, rpcHook));
    }
  }

 private:
  DefaultMQProducerImpl(DefaultMQProducerConfigPtr config);
  DefaultMQProducerImpl(DefaultMQProducerConfigPtr config, RPCHookPtr rpcHook);

 public:
  virtual ~DefaultMQProducerImpl();

 public:  // MQProducer
  void start() override;
  void shutdown() override;

  // Sync: caller will be responsible for the lifecycle of messages.
  SendResult send(MQMessage& msg) override;
  SendResult send(MQMessage& msg, long timeout) override;
  SendResult send(MQMessage& msg, const MQMessageQueue& mq) override;
  SendResult send(MQMessage& msg, const MQMessageQueue& mq, long timeout) override;

  // Async: don't delete msg object, until callback occur.
  void send(MQMessage& msg, SendCallback* sendCallback) noexcept override;
  void send(MQMessage& msg, SendCallback* sendCallback, long timeout) noexcept override;
  void send(MQMessage& msg, const MQMessageQueue& mq, SendCallback* sendCallback) noexcept override;
  void send(MQMessage& msg, const MQMessageQueue& mq, SendCallback* sendCallback, long timeout) noexcept override;

  // Oneyway: same as sync send, but don't care its result.
  void sendOneway(MQMessage& msg) override;
  void sendOneway(MQMessage& msg, const MQMessageQueue& mq) override;

  // Select
  SendResult send(MQMessage& msg, MessageQueueSelector* selector, void* arg) override;
  SendResult send(MQMessage& msg, MessageQueueSelector* selector, void* arg, long timeout) override;
  void send(MQMessage& msg, MessageQueueSelector* selector, void* arg, SendCallback* sendCallback) noexcept override;
  void send(MQMessage& msg,
            MessageQueueSelector* selector,
            void* arg,
            SendCallback* sendCallback,
            long timeout) noexcept override;
  void sendOneway(MQMessage& msg, MessageQueueSelector* selector, void* arg) override;

  // Transaction
  TransactionSendResult sendMessageInTransaction(MQMessage& msg, void* arg) override;

  // Batch: power by sync send, caller will be responsible for the lifecycle of messages.
  SendResult send(std::vector<MQMessage>& msgs) override;
  SendResult send(std::vector<MQMessage>& msgs, long timeout) override;
  SendResult send(std::vector<MQMessage>& msgs, const MQMessageQueue& mq) override;
  SendResult send(std::vector<MQMessage>& msgs, const MQMessageQueue& mq, long timeout) override;

  // RPC
  MQMessage request(MQMessage& msg, long timeout) override;
  void request(MQMessage& msg, RequestCallback* requestCallback, long timeout) override;
  MQMessage request(MQMessage& msg, const MQMessageQueue& mq, long timeout) override;
  void request(MQMessage&, const MQMessageQueue& mq, RequestCallback* requestCallback, long timeout) override;
  MQMessage request(MQMessage& msg, MessageQueueSelector* selector, void* arg, long timeout) override;
  void request(MQMessage& msg,
               MessageQueueSelector* selector,
               void* arg,
               RequestCallback* requestCallback,
               long timeout) override;

 public:  // MQProducerInner
  TransactionListener* getCheckListener() override;

  void checkTransactionState(const std::string& addr,
                             MessageExtPtr msg,
                             CheckTransactionStateRequestHeader* checkRequestHeader) override;

 public:
  void initTransactionEnv();
  void destroyTransactionEnv();

  const MQMessageQueue& selectOneMessageQueue(const TopicPublishInfo* tpInfo, const std::string& lastBrokerName);
  void updateFaultItem(const std::string& brokerName, const long currentLatency, bool isolation);

  void endTransaction(SendResult& sendResult,
                      LocalTransactionState localTransactionState,
                      std::exception_ptr& localException);

  bool isSendLatencyFaultEnable() const;
  void setSendLatencyFaultEnable(bool sendLatencyFaultEnable);

 protected:
  SendResult* sendDefaultImpl(MessagePtr msg,
                              CommunicationMode communicationMode,
                              SendCallback* sendCallback,
                              long timeout);
  SendResult* sendKernelImpl(MessagePtr msg,
                             const MQMessageQueue& mq,
                             CommunicationMode communicationMode,
                             SendCallback* sendCallback,
                             std::shared_ptr<const TopicPublishInfo> topicPublishInfo,
                             long timeout);
  SendResult* sendSelectImpl(MessagePtr msg,
                             MessageQueueSelector* selector,
                             void* arg,
                             CommunicationMode communicationMode,
                             SendCallback* sendCallback,
                             long timeout);

  TransactionSendResult* sendMessageInTransactionImpl(MessagePtr msg, void* arg, long timeout);
  void checkTransactionStateImpl(const std::string& addr,
                                 MessageExtPtr message,
                                 long tranStateTableOffset,
                                 long commitLogOffset,
                                 const std::string& msgId,
                                 const std::string& transactionId,
                                 const std::string& offsetMsgId);

  bool tryToCompressMessage(Message& msg);

  MessagePtr batch(std::vector<MQMessage>& msgs);

  void prepareSendRequest(Message& msg, long timeout);

 private:
  DefaultMQProducerConfigPtr m_producerConfig;
  std::unique_ptr<MQFaultStrategy> m_mqFaultStrategy;
  std::unique_ptr<thread_pool_executor> m_checkTransactionExecutor;
};

}  // namespace rocketmq

#endif  // __DEFAULT_MQ_PRODUCER_IMPL_H__
