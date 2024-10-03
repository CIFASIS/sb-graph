**Release procedure**

- Checkout `sb-graph-release` branch.
- Create branch `release-X.X.X`.
- Merge `sb-graph-dev` -> `sb-graph-release`.
- Update `version` file to `X.X.X`.
- Run `update.sh`.
- Commit update changes with `-n`.
- Update `CHANGELOG` and `README.md` files.
- Commit changes with `-n`.
- Push branch.
- Merge pushed branch in `sb-graph-release` with `--squash`.
- Push `sb-graph-release` branch.
- Merge `sb-graph-release` in `sb-graph-dev`.

**Github Realease**

- Select Releases -> Draft a new release.
- Choose the corresponding version tag of the new release.
- Select sb-graph-release as the target branch.
- Update release title and description.
- Upload deb files.
- Publish the release.
