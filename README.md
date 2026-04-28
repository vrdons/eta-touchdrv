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

Github actions would do work for you