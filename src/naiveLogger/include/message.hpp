#pragma once

#include <string>
#include "../utils/common.hpp"


namespace naiveLogger 
{

// log message
struct LogMessage {
    LogMessage() = default;
    LogMessage(log_clock::time_point log_time, source_location loc, string_view_t log_name, level_num lvl, string_view_t msg);
    LogMessage(source_location loc, string_view_t log_name, level_num lvl, string_view_t msg);
    LogMessage(string_view_t log_name, level_num lvl, string_view_t msg);

    LogMessage(const LogMessage &other) = default;
    LogMessage &operator=(const LogMessage &other) = default;

    source_location src_loc;
    string_view_t log_name;
    level_num level{level_num::off};
    log_clock::time_point time;

    size_t thread_id{0};

    string_view_t payload;

    
    // wrapping the formatted text with color (updated by pattern_formatter).
    // mutable size_t color_range_start{0};
    // mutable size_t color_range_end{0};


};

LogMessage::LogMessage(log_clock::time_point log_time, source_location loc, string_view_t log_name, level_num lvl, string_view_t msg): time(log_time), src_loc(loc), log_name(log_name), level(lvl), payload(msg) {
    this->thread_id = naiveLogger::nthread::getThreadId();
}

LogMessage::LogMessage(source_location loc, string_view_t log_name, level_num lvl, string_view_t msg): LogMessage(ntm::getNow(), loc, log_name, lvl, msg) {}

LogMessage::LogMessage(string_view_t log_name, level_num lvl, string_view_t msg): LogMessage(ntm::getNow(), source_location{}, log_name, lvl, msg) {}



// message buffer class
class MessageBuffer : public LogMessage {
public:
    MessageBuffer() = default;
    explicit MessageBuffer(const LogMessage &orig_msg);
    MessageBuffer(const MessageBuffer &msg_buf);
    // right value as param
    MessageBuffer(MessageBuffer &&other_msg_buf);
    MessageBuffer &operator=(const MessageBuffer &msg_buf);
    MessageBuffer &operator=(MessageBuffer &&other_msg_buf);


private:
    memory_buf_t buffer;
    void updateStringViews();

};

MessageBuffer::MessageBuffer(const LogMessage &orig_msg) : LogMessage(orig_msg) {
    buffer.append(log_name.begin(), log_name.end());
    buffer.append(payload.begin(), payload.end());
    updateStringViews();
}

MessageBuffer::MessageBuffer(const MessageBuffer &msg_buf) : LogMessage(msg_buf) {
    buffer.append(log_name.begin(), log_name.end());
    buffer.append(payload.begin(), payload.end());
    updateStringViews();
}

MessageBuffer::MessageBuffer(MessageBuffer &&other_msg_buf) : LogMessage(other_msg_buf), buffer(std::move(other_msg_buf.buffer)) {
    updateStringViews();
}

MessageBuffer &MessageBuffer::operator=(const MessageBuffer &msg_buf) {
    MessageBuffer::operator=(msg_buf);
    buffer.clear();
    buffer.append(msg_buf.buffer.data(), msg_buf.buffer.data() + msg_buf.buffer.size());
    updateStringViews();
    return *this;
}

MessageBuffer &MessageBuffer::operator=(MessageBuffer &&other_msg_buf) {
    MessageBuffer::operator=(other_msg_buf);
    buffer = std::move(other_msg_buf.buffer);
    updateStringViews();
    return *this;
}


void MessageBuffer::updateStringViews() {
    log_name = string_view_t{buffer.data(), log_name.size()};
    /* buffer.data is char *, + log_name.size() can point to the start position of payload */
    payload = string_view_t{buffer.data() + log_name.size(), payload.size()};

}


} // namespace naiveLogger