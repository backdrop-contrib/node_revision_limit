# Node Revision Limit
**Node Revision Limit** - Cap the number of previous node revisions kept per content type, reclaiming database space without losing revision history entirely.

Node Revision Limit adds a configurable revision cap to each content type. When a node is saved, any old revisions beyond the cap are pruned automatically. A cron queue handles bulk pruning when the limit is reduced across many existing nodes. Forward draft revisions - used by modules such as Draft Workflow — are never touched, making this module safe to use alongside any revision-based workflow.

## Why use this module?

Backdrop creates one revision per node regardless of whether revisions are enabled or disabled — it is a structural requirement of how nodes are stored. With revisions disabled, that revision is wasted: it takes up space but gives you nothing.

By enabling revisions and setting this module's limit to 1, you get a free undo. If an editor overwrites content by mistake, the previous version is one click away via the Revisions tab. The storage cost is similar either way — with revisions disabled, Backdrop stores a duplicate of the current content as a structural revision, wasting that space. With revisions enabled and a limit of 1, that same slot holds the previous version instead, giving you a free undo.

Use higher limits (2, 5, 10) where a fuller edit history matters. Leave blank for unlimited on content types where full history is important, such as legal pages or documentation.

## Features

* **Per Content Type Limit:** Set a maximum number of previous revisions to keep for each content type independently. Leave blank for unlimited (default Backdrop behaviour). Set to 0 to keep no previous revisions at all.
* **Automatic Pruning on Save:** Every time a node is saved, old revisions beyond the limit for that content type are deleted immediately. No manual intervention required.
* **Safe with Draft Workflow:** Only old revisions (older than the current revision) are ever pruned. Forward draft revisions — where a new draft has a higher revision ID than the published version — are never counted toward the limit or deleted.
* **Cron Queue for Bulk Operations:** When a limit is set or reduced on a content type, all existing nodes of that type are queued for pruning. The queue is processed in small chunks during each cron run, avoiding timeouts on large sites.
* **Admin Overview:** A summary page at Admin → Configuration → Content → Node Revision Limit shows each content type, its current limit, and total node count. A single button queues a prune pass for all content types that have a limit configured.
* **No Extra Tables:** Settings are stored within Backdrop's existing content type configuration (the same CMI pattern used by core and Draft Workflow). No additional database tables are created.
* **Backdrop Native:** Built for Backdrop CMS, not a port of a Drupal 7 module.

## Requirements

- Backdrop CMS 1.x
- PHP 8.0+

## Installation

Install this module using the official Backdrop CMS instructions at https://docs.backdropcms.org/documentation/extend-with-modules

No database updates or additional configuration is required after enabling the module.

## Configuration

1. Navigate to **Admin → Structure → Content Types** and edit a content type.
2. In the **Revision settings** section, enter a value in the **Maximum previous revisions to keep** field.
   - Leave blank for unlimited (no change to existing behaviour).
   - Set to `1` to keep one previous revision (one undo/revert).
   - Set to `0` to keep no previous revisions (current state only).
3. Save the content type. If the new limit is lower than the previous setting, all nodes of that type will be queued for pruning automatically.
4. Run cron to process the prune queue, or use the **Queue prune for all limited types now** button at **Admin → Configuration → Content → Node Revision Limit**.

Existing old revisions that exceed the new limit will be removed during the next cron run. Future saves will prune automatically without cron.

## Compatibility with similar modules

| Module | Status |
|---|---|
| Draft Workflow | Safe - forward draft revisions are never pruned |
| Revisioning | Safe - forward pending revisions are never pruned |
| Enforce Revlog | No conflict |
| Node Revision History | Complementary - useful for one-off backlog cleanup |
| Diff | No conflict - fewer revisions means fewer diffs available |

## Issues

Bugs and feature requests should be reported in the Issue Queue on GitHub.

## Current Maintainer(s)
- Steve Moorhouse (albanycomputers) (https://github.com/albanycomputers)
- Additional maintainers and contributors welcome.

## Credits
- Steve Moorhouse - Zulip (DrAlbany)
- Claude Code by Anthropic assisted with development of this module.

- Current development is sponsored by [Albany Computer Services](https://www.albany-computers.co.uk), providers of computer support, [web design](https://www.albanywebdesign.co.uk), and [web hosting](https://www.albanywebdesign.co.uk).

## License
This project is GPL v2 or later software. See the LICENSE.txt file in this directory for complete text.
