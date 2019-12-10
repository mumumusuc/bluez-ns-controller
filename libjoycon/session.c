#include <assert.h>
#include "defs.h"
#include "session.h"
#include "input_report.h"
#include "output_report.h"

#define assert_session(session) assert(session && session->recv && session->send)

/*extern*/ struct Session
{
    Recv *recv;
    Send *send;
    InputReport_t *input;
    OutputReport_t *output;
    Device_t host;
};

Session_t *Session_create(Device_t *host, Recv *recv, Send *send)
{
    _FUNC_;
    Session_t *session;
    assert(host && recv && send);
    //assert(host->role == CONSOLE);
    session = (Session_t *)malloc(sizeof(Session_t));
    if (!session)
        goto done;
    bzero(session, sizeof(Session_t));
    session->recv = recv;
    session->send = send;
    memmove(&session->host, host, sizeof(Device_t));
    session->output = createOutputReport(NULL);
    if (!session->output)
        goto free;
    session->input = createInputReport(NULL);
    if (!session->input)
        goto free;
    return session;

free:
    Session_release(session);
done:
    perror("error when create session\n");
    return NULL;
}

void Session_release(Session_t *session)
{
    _FUNC_;
    if (session)
    {
        free(session->output);
        free(session->input);
    }
    free(session);
}
