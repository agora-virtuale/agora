# Security policy

## Supported code

Security reports are assessed against the current `main` branch. Agora is an early research exchange simulation; there are no separately maintained release branches or production-support commitments. When reporting an older commit, include its hash and whether the issue also occurs on current `main`.

## Report a vulnerability privately

Use [GitHub's private vulnerability reporting form](https://github.com/agora-virtuale/agora/security/advisories/new), also available under **Security → Report a vulnerability**. This sends the report privately to repository maintainers. Do not put vulnerability details, exploit material, credentials, or private data in public issues or pull requests.

Include, where available:

- The affected commit, component, operating system, and compiler/toolchain versions.
- A minimal reproduction using synthetic inputs and the steps needed to trigger it.
- The expected behavior, observed impact, and relevant sanitized diagnostics.
- Any suggested mitigation and whether you can help validate a fix.

If the private form is unavailable, open a public issue titled **Request private security contact**, without technical details or sensitive information, so a maintainer can arrange a private channel.

## Assessment and disclosure

Maintainers will assess the report, request clarification as needed, and coordinate any fix and disclosure with the reporter. This is a small volunteer project; there is no guaranteed response or remediation timeline and no bug bounty. Ask before testing against systems you do not own or control. Prefer local, minimal reproductions.

Report ordinary build failures, documented TODO exceptions, and non-security correctness bugs through the regular bug-report form. If unsure whether an issue exposes data, enables unintended execution, or creates a security boundary failure, use the private route.
