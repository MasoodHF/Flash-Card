# Flash-Card

Adding, editing and removing words and their explanations in order to revise them automatically
It works on Terminal on Linux and Windows OSes. The main menu shows the Commands.

## User manual

By adding you can enter a new Word, its Meaning and its Example in separate Entries.
By deleting Command, the entire buffer will be checked for finding the Word.
- After approving deletion, it will remove the entire Filed of that Word.
BY editing Command you can change them respectively, but by pressing ENTER and leave it blank it won't change.
- On the other hand, if you want to remove a Word, its definition or its example you have to enter at least one space.
It will remove extra spaces following the end of string.
By revising, it looks for every Word that has been reached to the revise time.
- Assume that every time you remember the Definition and the Example of the Word showed,
  the next rives time would be twice longer than the previous one.
- For a new Word, the first revise time is the next day, and the second revise time
  is two days later after the last revision, and so on...(1,2,4,8,16,32,64,128,256...)
- After eighth or ninth step, if you passed them respectively, of course,
  you can not only be sure that you never forget that Word, but also you are able to use them actively.

The project is steel in progress.
