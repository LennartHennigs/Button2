---
name: release
description: Cut a new Button2 release — suggests version bump based on change type, updates version files and CHANGELOG, commits, tags, pushes, creates a GitHub release, and publishes to PlatformIO
---

Cut a new Button2 release. Follow these steps exactly:

## Step 1: Determine the version

Read `library.json` to get the current version.

Read the `## Unreleased` section of `CHANGELOG.md` to understand what changed.

If $ARGUMENTS is provided, use that as the new version. Otherwise:
- Classify the unreleased changes:
  - **Major** (x.0.0): breaking API changes, removed functions, changed signatures
  - **Minor** (x.y.0): new public functions, new features, new examples
  - **Patch** (x.y.z): bug fixes, internal changes, test additions, docs
- Suggest the new version with a one-line explanation of your reasoning
- Use AskUserQuestion to ask the user to confirm the suggested version or provide a different one

## Step 2: Update version files

Update the version number in **both**:
- `library.json` — the `"version"` field
- `library.properties` — the `version=` line

## Step 3: Finalize CHANGELOG

In `CHANGELOG.md`:
- Rename `## Unreleased` to `## [x.y.z] - YYYY-MM-DD` (use today's date and the new version)
- Add a fresh `## Unreleased` section above it (empty, just the heading)

## Step 4: Commit, tag, push

```bash
git add library.json library.properties CHANGELOG.md
git commit -m "chore: release vX.Y.Z"
git tag vX.Y.Z
git push && git push --tags
```

## Step 5: Create GitHub release

Extract the release notes for this version from the CHANGELOG (the bullet points under `## [x.y.z]`), then run:

```bash
gh release create vX.Y.Z \
  --title "vX.Y.Z" \
  --notes "<changelog bullet points for this version>"
```

## Step 6: Publish to PlatformIO registry

```bash
pio pkg publish --no-interactive
```

This pushes directly to the PlatformIO registry. The Arduino Library Manager picks up the new tag automatically from GitHub — no separate step needed there.

Confirm each step succeeded and report which registries were updated.
