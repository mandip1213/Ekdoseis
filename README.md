Commands:
# 1. init

Just do `dose init` and you are ready to explore.

# 2. status 
The `status` command let you know about the current `status` of all files in the repo which may be:
- untracked
- modified
- staged
- commited
- ignored

There are no `flags/options` available for this command.

# 3. log
The `log` command simply display all your commits in the current branch.
There are no `flags/options` available for this command.

Now comes the command that does something useful other than printing vague in your terminal.

# 4.  add
`dose add [<filepath>..]`
The `add` command will simply adds the `file/s` in the staging area.
You can run `status` command before and after running `add` to view the difference.

There are no `flags/options` available for this command.

- You can provide multiple path to add like `dose add a.txt b.txt c/d.txt`. 

> Note: Please donot use `.` where path is expected. It is an [undefined behaviour](https://en.wikipedia.org/wiki/Undefined_behavior) and you will be responsible for the damage caused by this. 

# 5. commit
`dose commit -m <message>`
To capture a snapshot of the files you have staged, you need to commit the staged files. You can do so with the command `commit`. 

There are no other flavours of the command except included just below the heading.

# 6. restore
`dose restore [--staged] [<filepath>..]`
This command is useful when you want to remove file from staging are or you want to ignore the changes you have made in your file.

- `--staged`: With this flag, the `filepath` will be removed from staging area. The changes will persist.

- With no flag, the any chnage in `filepath` will be restored to the latest commit, pointed by the `HEAD`.

- Just like with `add` command, you can provide multiple  filepaths and directory path.

- Like above, the use of `.` as file path is an undefined behaviour.

# 7. branch
Finally, the thing you have been searching for. And guess what, it comes in different varieties.
`dose branch [-l | --list] [-d | --delete] [<branch-name>] [<existing-branch> | <commit-hash>]`

- `dose branch` | `dose branch -l` | `dose branch --list`
The command list all the branches in your repo. As simple as that.

- `dose branch [-d | --delete] <branch-name>`
The above command will delete the branch `<branch-name>` if it exists.

-`dose branch <branch-name>`
If you want to create a new branch at current `HEAD` you can simply run above command.
- `dose branch <branch-name> <old-branch>`
The command mentioned above will create a new branch with name `<branch-name>` and points to the commit referred by `<old-branch>` instead of ideal `HEAD` pointer.
You can also use commit hash instead of existing branch name in the above command.

# 8. checkout
Branching is not fun when you cannot randomly switch from one branch into another to get out of your boredom. With this in mind we present you with the penultimate command.
`dose checkout [<branch-name> | <commit-hash>]`
- If the checkout point is `branch-name` your current working repo will  be updated with the files present in the commit referenced by the branch and the `HEAD` pointer will point to the new branch.

- If the chekcout point is `commit-hash` the head pointer refers directly point to the `commit-hash`. This particular chaotic disorder is termed as `detached head` state. And you are recommended to staty out of this, unless you become familiar with the basics of version control system.


# 9. merge
What's the essence of having branch if you cannot merge them? Well, here we are with the final feature of our VCS.
`dose merge [<branch-name>]`
The above command will merge the branch `branch-name` into the current branch.
> You cannot merge while you are in `detached HEAD` state.





## SETUP
- Clone the repo if you havenot already.
- Go to the repo folder and open `Ekdoseis.soln` file. This will open the project in visual studio.
- First build the solution in visual studio with `shift+ctrl+b`.
- Open ./x64/Debug and copy the location of the folder.
- Add the copied path to your environment variables.
- copy the dose.bat file in C:/winows(on windows)

open the terminal and you are ready to go.
The root command is `dose`.
You can start with `dose init`.
