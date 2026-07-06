/** @file
 * @brief LDAP Group
 *
 * Bind to a directory and search it. The directory is the test's to
 * supply (env TSF_LDAP_URI, e.g. "ldap://host"); with none configured
 * the test skips cleanly. Optional env: TSF_LDAP_BASE (search base),
 * TSF_LDAP_BINDDN + TSF_LDAP_PW (a simple bind to try in addition to
 * the anonymous one). Read-only.
 *
 * Copyright (C) 2026 Interpretica Unipessoal Lda
 */

#define TE_TEST_NAME    "ldap/probe"

#include "te_config.h"
#include <stdlib.h>
#include "tapi_test.h"
#include "te_string.h"

#include "tapi_ldap.h"
#include "tsapi_ldap.h"

int
main(int argc, char **argv)
{
    tsapi_ldap_session sess = {0};
    const char *uri;
    const char *base;
    const char *binddn;
    const char *pw;
    int code = 0;
    int count = 0;
    te_string detail = TE_STRING_INIT;
    te_string dns = TE_STRING_INIT;

    TEST_START;

    uri = getenv("TSF_LDAP_URI");
    if (uri == NULL || uri[0] == '\0')
        TEST_SKIP("Set TSF_LDAP_URI (e.g. ldap://host) to point at a directory");
    base = getenv("TSF_LDAP_BASE");
    binddn = getenv("TSF_LDAP_BINDDN");
    pw = getenv("TSF_LDAP_PW");

    TEST_STEP("Open a session to the agent");
    CHECK_RC(tsapi_ldap_session_init(&sess, "pco_ldap_probe"));

    TEST_STEP("Anonymous bind to %s", uri);
    CHECK_RC(tapi_ldap_bind(sess.pco, uri, NULL, NULL, false, &code, &detail));
    RING("anonymous bind -> LDAP result %d%s%s", code,
         detail.len != 0 ? ": " : "",
         detail.len != 0 ? te_string_value(&detail) : "");

    TEST_STEP("Anonymous search of '%s'", base != NULL ? base : "(root DSE)");
    CHECK_RC(tapi_ldap_search(sess.pco, uri, NULL, NULL, false,
                              base != NULL ? base : "",
                              TAPI_LDAP_SCOPE_BASE, NULL, &count, &dns));
    RING("search returned %d entr%s:\n%s", count, count == 1 ? "y" : "ies",
         te_string_value(&dns));

    if (binddn != NULL && binddn[0] != '\0')
    {
        TEST_STEP("Simple bind as %s", binddn);
        te_string_reset(&detail);
        CHECK_RC(tapi_ldap_bind(sess.pco, uri, binddn, pw, false, &code,
                                &detail));
        RING("simple bind -> LDAP result %d", code);
    }

    TEST_SUCCESS;

cleanup:
    te_string_free(&detail);
    te_string_free(&dns);
    tsapi_ldap_session_fini(&sess);
    TEST_END;
}
