# Maintenance

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
