# Accessibility

Agora aims to make its documentation, contribution process, and developer tools usable by people with different access needs.

## Current interfaces and environments

The project currently provides C++ source, Markdown documentation, and a command-line scaffold. It has no graphical trading interface. Windows and Linux builds run in CI; local setup is documented in the README. The command-line program uses plain text for its current status, help, and foundation demo.

Accessibility also depends on the terminal, editor, assistive technology, and GitHub interface used to access the project. Those combinations have not undergone a dedicated project accessibility audit, and we do not claim conformance to a specific accessibility standard.

## Contribution goals

- Use descriptive headings, meaningful link text, and readable text examples.
- Explain diagrams in surrounding text and provide alternative text for informative images.
- Keep commands copyable and provide text diagnostics instead of requiring screenshots.
- Avoid conveying meaning through color alone or requiring a particular graphical editor.
- Consider keyboard navigation, assistive technology, contrast, and reduced motion when a future interface is designed.

## Report a barrier

Use the [accessibility issue form](https://github.com/agora-virtuale/agora/issues/new?template=accessibility.yml). Describe the task, the barrier, the page or command involved, and what would help. OS, terminal/editor, or assistive-tool details are optional; do not include medical information or other private data.

Maintainers will discuss practical improvements through the issue. If public reporting is unsuitable, request a private maintainer contact without posting the underlying details. Accessibility fixes are welcome as focused contributions.
