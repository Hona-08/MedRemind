# MedRemind — Medication Reminder SaaS (Drupal 7)

A complete medication reminder and tracking application built as a custom Drupal 7 module.

## Features

- **Medication CRUD** — Add, edit, delete medications with dosage and schedule
- **AJAX Dashboard** — Take/Skip doses without page reload
- **Streak Tracking** — Consecutive day tracking with milestone notifications
- **Cron Reminders** — Automated email reminders and auto-missed dose marking
- **On-Site Notifications** — Bell icon with unread badge and dropdown
- **Reports & Analytics** — Adherence charts, medication breakdown, streak stats
- **CSV Export/Import** — Export medications and history, import from CSV
- **Admin Settings** — Global configuration with live system statistics
- **30 Automated Tests** — SimpleTest test cases covering all functionality
- **Responsive Design** — Works on desktop and mobile

## Technical Details

| Metric | Count |
|--------|-------|
| Database Tables | 7 |
| Routes/Endpoints | 17 |
| Template Files | 3 |
| Include Files | 8 |
| Test Cases | 30 |
| CSS Lines | 1200+ |

**Architecture:** Custom tables with db_select/db_insert (not Entity API)

## Database Tables

1. `medremind_medications` — Medication data
2. `medremind_schedule` — Dose times per medication
3. `medremind_log` — Action history (taken/missed/skipped)
4. `medremind_reminders` — Email reminder queue
5. `medremind_streaks` — Streak tracking per medication
6. `medremind_settings` — User preferences
7. `medremind_notifications` — On-site notifications

## Routes

| Path | Description |
|------|-------------|
| /medremind | Dashboard |
| /medremind/add | Add medication form |
| /medremind/edit/% | Edit medication |
| /medremind/delete/% | Delete confirmation |
| /medremind/history | Dose history |
| /medremind/settings | User settings |
| /medremind/reports | Reports & analytics |
| /medremind/notifications | All notifications |
| /medremind/export | Export/Import page |
| /medremind/take/%/ajax | AJAX: Take dose |
| /medremind/skip/%/ajax | AJAX: Skip dose |
| /admin/config/medremind | Admin settings |

## Installation

1. Copy `medremind/` to `sites/all/modules/`
2. Enable at Admin → Modules
3. Place sidebar blocks at Admin → Structure → Blocks
4. Configure at Admin → Configuration → MedRemind Settings
5. Set front page to `medremind` at Admin → Configuration → Site Information

## File Structure
```
medremind/
├── medremind.info
├── medremind.module
├── medremind.install
├── medremind.test
├── medremind.css
├── README.md
├── .gitignore
├── js/
│   └── medremind.js
├── includes/
│   ├── medremind.pages.inc
│   ├── medremind.forms.inc
│   ├── medremind.admin.inc
│   ├── medremind.ajax.inc
│   ├── medremind.blocks.inc
│   ├── medremind.cron.inc
│   ├── medremind.notifications.inc
│   ├── medremind.reports.inc
│   └── medremind.export.inc
└── templates/
    ├── medremind-dashboard.tpl.php
    ├── medremind-med-card.tpl.php
    └── medremind-history.tpl.php
```

## Development Steps

1. Module setup, routes, permissions
2. Database schema (6 tables → 7 tables)
3. Templates and theming
4. Add/Edit/Delete forms with validation
5. Dashboard with AJAX Take/Skip
6. Sidebar blocks (Next Dose + Quick Stats)
7. Admin settings with system statistics
8. Cron & Email reminders
9. On-site notification system
10. Reports & Analytics
11. CSV Export/Import
12. Final polish & navigation

## Testing
```bash
# Run all tests
php scripts/run-tests.sh --group "MedRemind"

# Or via UI
Admin → Configuration → Development → Testing → MedRemind
```

## Author

Built by Hona as a portfolio project demonstrating Drupal 7 custom module development.

## License