/** @file
 * @brief LDAP Group
 *
 * Read a directory's LDAP posture with tapi_ldap_audit() and gate on
 * it. The directory is the test's to supply (env TSF_LDAP_URI); with
 * none configured the test skips. What the findings are depends on the
 * directory, so this asserts a well-formed report and fails only on a
 * finding at least HIGH.
 *
 * Authorized use only: it binds to and reads a real directory.
 *
 * Copyright (C) 2026 Interpretica Unipessoal Lda
 */

#define TE_TEST_NAME    "ldap/audit"

#include "te_config.h"
#include <stdlib.h>
#include "tapi_test.h"
#include "te_string.h"

#include "tapi_cybersec.h"
#include "tapi_ldap_audit.h"
#include "tsapi_ldap.h"

int
main(int argc, char **argv)
{
    tsapi_ldap_session sess = {0};
    tapi_cybersec_report report;
    te_string verdict = TE_STRING_INIT;
    const char *uri;
    bool report_ready = false;

    TEST_START;

    uri = getenv("TSF_LDAP_URI");
    if (uri == NULL || uri[0] == '\0')
        TEST_SKIP("Set TSF_LDAP_URI (e.g. ldap://host) to point at a directory");

    TEST_STEP("Open a session to the agent");
    CHECK_RC(tsapi_ldap_session_init(&sess, "pco_ldap_audit"));

    TEST_STEP("Read the LDAP posture of %s into a report", uri);
    tapi_cybersec_report_init(&report);
    report_ready = true;
    CHECK_RC(tapi_ldap_audit(sess.pco, uri, NULL, &report));
    tapi_cybersec_report_log(&report);

    TEST_STEP("The report is well-formed");
    if (tapi_cybersec_report_count(&report, TAPI_CYBERSEC_SEV_INFO) == 0)
        TEST_VERDICT("the LDAP audit produced no findings at all");

    TEST_STEP("Gate: fail on anything at least HIGH");
    if (tapi_cybersec_report_verdict(&report, TAPI_CYBERSEC_SEV_HIGH, &verdict))
        TEST_VERDICT("%s", verdict.ptr);

    TEST_SUCCESS;

cleanup:
    te_string_free(&verdict);
    if (report_ready)
        tapi_cybersec_report_free(&report);
    tsapi_ldap_session_fini(&sess);
    TEST_END;
}
