"""Check generated reference files and local link fragments without network access."""

import argparse
from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit


class Page(HTMLParser):
    def __init__(self):
        super().__init__()
        self.anchors = set()
        self.links = set()

    def handle_starttag(self, tag, attributes):
        attributes = dict(attributes)
        for name in ("id", "name"):
            if attributes.get(name):
                self.anchors.add(attributes[name])
        for name in ("href", "src"):
            if attributes.get(name):
                self.links.add(attributes[name])


def check(root):
    pages = {}
    for path in root.rglob("*.html"):
        page = Page()
        page.feed(path.read_text(encoding="utf-8"))
        pages[path] = page
    if not pages:
        raise SystemExit(f"No HTML pages found in {root}")

    targets = {}
    errors = []
    for source, page in pages.items():
        for link in sorted(page.links):
            url = urlsplit(link)
            if url.scheme or url.netloc:
                continue
            key = (source.parent, url.path) if url.path else (source, "")
            if key not in targets:
                target = (source.parent / unquote(url.path)).resolve() if url.path else source
                if target.is_dir():
                    target /= "index.html"
                targets[key] = (target, target.is_file())
            target, exists = targets[key]
            reason = None
            if not exists:
                reason = "missing file"
            elif url.fragment and target in pages:
                if unquote(url.fragment) not in pages[target].anchors:
                    reason = "missing anchor"
            if reason:
                errors.append(f"{source.relative_to(root)}: {link!r} ({reason})")
    if errors:
        raise SystemExit("Broken reference links:\n" + "\n".join(errors))
    print(f"Reference links passed: {len(pages)} HTML pages, all local files and anchors resolve.")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("html", type=Path, help="Generated HTML directory")
    check(parser.parse_args().html.resolve())


if __name__ == "__main__":
    main()
