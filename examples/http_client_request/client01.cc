#include <stdio.h>
#include <stdlib.h>

#include <evpp/event_loop_thread.h>

#include <evpp/httpc/request.h>
#include <evpp/httpc/conn.h>
#include <evpp/httpc/response.h>

#include "../echo/tcpecho/winmain-inl.h"

static bool responsed = false;
static void HandleHTTPResponse(const std::shared_ptr<evpp::httpc::Response>& response, evpp::httpc::GetRequest* request) {
    LOG_INFO << "http_code=" << response->http_code() << " [" << response->body().ToString() << "]";
    std::string header = response->FindHeader("Connection");
    LOG_INFO << "HTTP HEADER Connection=" << header;
    responsed = true;
    assert(request == response->request());
    delete request; // The request MUST BE deleted in EventLoop thread.
}

int main() {
    evpp::EventLoopThread t;
    t.Start(true);
    evpp::httpc::GetRequest* r = new evpp::httpc::GetRequest(t.event_loop(), "http://www.360.cn/robots.txt", evpp::Duration(2.0));
    LOG_INFO << "Do http request";
    r->Execute(std::bind(&HandleHTTPResponse, std::placeholders::_1, r));

    while (!responsed) {
        usleep(1);
    }

    t.Stop(true);
    LOG_INFO << "EventLoopThread stopped.";
    return 0;
}
