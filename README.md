# tsf-ldap-ts

A Test Environment suite that exercises
[tsf-ldap](https://github.com/interpretica-io/tsf-ldap) (`tapi_ldap`)
against an LDAP directory reachable from the agent it runs on, over
OpenLDAP in the agent's RPC server.

| Test | What it checks |
|---|---|
| `probe` | an anonymous bind and a search (root DSE or `TSF_LDAP_BASE`); optionally a simple bind with `TSF_LDAP_BINDDN`/`TSF_LDAP_PW` |
| `audit` | `tapi_ldap_audit()` produces a well-formed posture report (anonymous bind, cleartext bind, anonymous-readable base) and the gate fails on any finding ≥ HIGH |

Both take the directory from **env `TSF_LDAP_URI`** (e.g. `ldap://host`);
with none set the tests `TEST_SKIP` cleanly, so a bare run is green.

## Authorized use only

The suite binds to and reads a real directory. Point it only at one you
own or are engaged to test.

## Running it

```bash
./scripts/run.sh guess --cfg=localhost      # native; or prefix 'docker'
```

Needs `test-environment` as a sibling and the agent host carrying
`libldap2-dev`. The Builder resolves tsf-ldap and the tsf-cybersec chain
from `conf/external.yml`.

## Status

Written alongside tsf-ldap; verified by building and running natively
(the module's OpenLDAP usage was syntax-checked against the real
headers). With no `TSF_LDAP_URI` the suite skips; point it at a
directory to exercise the bind/search/audit paths.
