# ETA-TOUCHDRV

**NOTE:** This project is fork of [pardus/eta-touchdrv](github.com/pardus/eta-touchdrv)

## Introduction

eta-touchdrv project provides kernel modules and their corresponding daemons for
non-hid 2-camera and 4-camera touchscreen sensors of Fatih Interactive White
Boards. Kernel modules are open-source, but source code of server daemons are
unavailable. They are provided by Vestel.

## Maintenance

Create topic branches (i.e. topic/bugfix, topic/new-feature) off the master.

```bash
git checkout -b topic/bugfix master
# hack hack hack to fix the bug
```

Implement the feature or fix the bug and merge it back to master if it passes
all the tests.

```bash
# Add and commit new changes
git commit -am "New changes"
# In case remote master changed
git checkout master
git pull
# Rebase topic branch
git checkout topic/bugfix
git rebase master
# Merge topic branch or create pull request
git checkout master
git merge topic/bugfix --no-ff
```

## Packaging

GitHub Actions now handles Debian release artifacts and AUR publishing:

- `.github/workflows/Release.yml` builds `.deb` artifacts and attaches them to GitHub releases when a `v*.*.*` tag is pushed.
- `.github/workflows/aur.yml` validates AUR PKGBUILDs on pull requests, publishes stable `eta-touchdrv` from GitHub Release events (`published`), and publishes `eta-touchdrv-git` on every commit to `master`.

AUR packaging files live under `package/aur/`:

- `package/aur/eta-touchdrv/PKGBUILD`: stable package sourced from the published Debian `.deb` release asset.
- `package/aur/eta-touchdrv-git/PKGBUILD`: development git package.

### One-time AUR setup

1. Create package bases on AUR first (`eta-touchdrv` and `eta-touchdrv-git`) so CI can push updates.
2. Add the deploy key public part to your AUR account.
3. Configure these GitHub repository secrets:
   - `AUR_USERNAME`
   - `AUR_EMAIL`
   - `AUR_SSH_PRIVATE_KEY`

After this, each published GitHub Release updates the stable AUR package, and each commit to `master` updates the `-git` AUR package automatically.
