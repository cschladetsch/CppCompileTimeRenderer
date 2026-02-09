#!/usr/bin/env python3
import re
import sys

TAG_RE = re.compile(r"LineTag<([^>]*)>")
CHAR_RE = re.compile(r"'(.)'")


def extract_lines(text: str):
    lines = []
    for match in TAG_RE.finditer(text):
        payload = match.group(1)
        chars = CHAR_RE.findall(payload)
        if chars:
            lines.append("".join(chars))
    return lines


def main():
    data = sys.stdin.read()
    lines = extract_lines(data)
    if not lines:
        return 1
    sys.stdout.write("\n".join(lines))
    if not data.endswith("\n"):
        sys.stdout.write("\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
