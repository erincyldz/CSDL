# Contribution Guidelines

Thank you for considering contributing to our organization! Please follow these steps:

## Issues
- Use appropriate templates for bug reports or feature requests.
- Provide as much detail as possible.

## Pull Requests
- Ensure your PR is linked to an issue.
- Follow the coding standards outlined in the README.
- Use the PR template for consistency.

## Commit Messages
Commit messages should be self explanatory with enough detail for developers, maintainers, testers and every other member of the development team to understand what exactly has changed with each commit.
- Commit type that will give first hints about commit change.
    - "[+]": New feature
    - "[#]": Bug fix
    - "[~]": Refactoring, general changes or others that doesn't fit first two 
- Summary is the must have and it has two parts in it. First part is the name of the module and the second part is the summary of the change.
- Description is made out of several parts, it can be considered as a template of explaining, giving enough detail on the changes about:
    - how algorithm works
    - what are the new dependencies
    - how this change solves the problem/creates solution
    - additional notes
    - relevant links (issues, other PRs etc.)
    - co-authors.

- Use the imperative mood in the summary line while writing commit messages. A properly written summary line for a commit message should be able to complete the following sentence: *"This commit will ..."*.

    - This commit will *change the search algorithm*.
    - This commit will ~~*the search algorithm is changed*~~.

Other than summary, all the parts are optional yet, it is highly encouraged to use them. Template and example can be found below:

**Template:**
```markdown
[~#+] Module Name Abbreviation(if needed): Summarize the change in less than 50 characters

What is changed (if needed):
- Explain new algorithm.
- Explain new dependencies.

Because (if needed):
- Explain the reasons you made this change
- Make a new bullet for each reason
- Each line should be under 72 characters
Remarks (if needed):
Include any additional notes, relevant links, or co-authors.(if needed)

Issue: #GITHUB_ISSUE_ID
```

**Example:**

```markdown
[~] search: Refactor algorithm

What is changed:
- The new algorithm's worst case O(n log(n)).
- "HyperSupperDupper Sort" library is not used anymore.

Because:
- Old algorithm was not optimized.
- System tend to crash with big data.

Remarks:
Corner cases were not covered. (see # 11231)

These changes should resolve issue # 1337.

This commit removed the getFirst(), so please stop using it!

Issue: #2019
```
Check [this](https://cbea.ms/git-commit/) article to read more about writing good commit messages.
Another cool git commit can be found [here](https://github.com/bitcoin/bitcoin/commit/eb0b56b19017ab5c16c745e6da39c53126924ed6).
