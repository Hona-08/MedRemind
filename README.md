# MedRemind — Medication Reminder SaaS (Drupal 7)

A complete medication reminder and tracking application built as a custom Drupal 7 module, demonstrating both **Custom Tables** and **Entity API** approaches.

---

## Features

- **Medication CRUD** — Add, edit, delete medications with dosage, frequency, and multi-time scheduling
- **AJAX Dashboard** — Take/Skip doses without page reload with real-time stat updates
- **Streak Tracking** — Consecutive day tracking with milestone notifications (7, 14, 30, 60, 90, 365 days)
- **Cron Reminders** — Automated email reminders, auto-missed dose marking, daily summary emails
- **On-Site Notifications** — Bell icon with unread badge, dropdown preview, full notifications page
- **Reports & Analytics** — Adherence charts, per-medication breakdown, streak leaderboard
- **CSV Export/Import** — Export medications/history/full data, import from CSV with validation
- **Prescription Entity** — Entity API-based prescription tracking with refills and expiry
- **Admin Settings** — Global configuration with live system statistics
- **37 Automated Tests** — SimpleTest test cases covering all functionality
- **Responsive Design** — Works on desktop, tablet, and mobile

---

## Technical Details

| Metric | Count |
|--------|-------|
| Database Tables | 8 (7 custom + 1 entity) |
| Routes/Endpoints | 18+ |
| Template Files | 3 |
| Include Files | 10 |
| Test Cases | 37 |
| CSS Lines | 1300+ |

**Architecture:** Custom Tables (db_select/db_insert) + Entity API (entity_save/entity_load)

---

## Database Tables

| # | Table | Type | Purpose |
|---|-------|------|---------|
| 1 | `medremind_medications` | Custom | Medication data |
| 2 | `medremind_schedule` | Custom | Dose times per medication |
| 3 | `medremind_log` | Custom | Action history (taken/missed/skipped) |
| 4 | `medremind_reminders` | Custom | Email reminder queue |
| 5 | `medremind_streaks` | Custom | Streak tracking per medication |
| 6 | `medremind_settings` | Custom | User preferences |
| 7 | `medremind_notifications` | Custom | On-site notifications |
| 8 | `medremind_prescriptions` | Entity | Prescription records (Entity API) |

---

## Routes

| Path | Description |
|------|-------------|
| `/medremind` | Dashboard |
| `/medremind/add` | Add medication form |
| `/medremind/edit/%` | Edit medication |
| `/medremind/delete/%` | Delete confirmation |
| `/medremind/history` | Dose history |
| `/medremind/settings` | User settings |
| `/medremind/reports` | Reports & analytics |
| `/medremind/notifications` | All notifications |
| `/medremind/export` | Export/Import page |
| `/medremind/prescriptions` | Prescriptions (Entity) |
| `/medremind/prescription/add` | Add prescription |
| `/medremind/take/%/ajax` | AJAX: Take dose |
| `/medremind/skip/%/ajax` | AJAX: Skip dose |
| `/medremind/export/medications/csv` | Download medications CSV |
| `/medremind/export/history/csv` | Download history CSV |
| `/medremind/export/full/csv` | Download full export CSV |
| `/admin/config/medremind` | Admin settings |

---

## File Structure

```
medremind/
├── medremind.info                      # Module registration
├── medremind.module                    # All hooks
├── medremind.install                   # Schema (8 tables), install, uninstall, updates
├── medremind.test                      # 37 SimpleTest test cases
├── medremind.css                       # Complete stylesheet (1300+ lines)
├── README.md
├── .gitignore
├── js/
│   └── medremind.js                    # AJAX Take/Skip handlers
├── includes/
│   ├── medremind.pages.inc             # Dashboard + History page callbacks
│   ├── medremind.forms.inc             # Add/Edit/Delete/Settings forms
│   ├── medremind.admin.inc             # Admin settings (system_settings_form)
│   ├── medremind.ajax.inc              # Take/Skip AJAX endpoints + streak logic
│   ├── medremind.blocks.inc            # Sidebar blocks + notification bell
│   ├── medremind.cron.inc              # Reminders, auto-miss, cleanup, notifications
│   ├── medremind.notifications.inc     # Notification CRUD + pages + AJAX
│   ├── medremind.reports.inc           # Reports with charts + streaks
│   ├── medremind.export.inc            # CSV export/import + template
│   ├── medremind.entity.inc            # Prescription Entity class + controllers
│   └── medremind.prescription.pages.inc # Prescription user pages + forms
└── templates/
    ├── medremind-dashboard.tpl.php     # Dashboard with stats bar
    ├── medremind-med-card.tpl.php      # Single medication card
    └── medremind-history.tpl.php       # History with adherence circle
```

---

## Development Steps

| Step | Feature | Approach |
|------|---------|----------|
| 1 | Module setup, routes, permissions | hook_menu, hook_permission |
| 2 | Database schema (6 tables) | hook_schema, db_select/db_insert |
| 3 | Templates and theming | hook_theme, .tpl.php files |
| 4 | Add/Edit/Delete forms | Form API with validation |
| 5 | Dashboard with AJAX Take/Skip | jQuery + drupal_json_output |
| 6 | Sidebar blocks | hook_block_info/view |
| 7 | Admin settings | system_settings_form |
| 8 | Cron & Email reminders | hook_cron, hook_mail |
| 9 | On-site notifications | Custom notification system |
| 10 | Reports & Analytics | Pure CSS charts |
| 11 | CSV Export/Import | fputcsv, fgetcsv |
| 12 | Final polish & navigation | Tab navigation |
| 13 | Prescription Entity | Entity API module |

---

## Custom Tables vs Entity API

This module demonstrates **both approaches** side by side:

| Feature | Custom Tables | Entity API |
|---------|--------------|------------|
| Used for | Medications, Log, Streaks | Prescriptions |
| CRUD | Manual db_insert/update/delete | entity_save/load/delete |
| Forms | Manual Form API | Auto-generated |
| Admin UI | Custom pages | EntityDefaultUIController |
| Fieldable | No | Yes (attach fields via UI) |
| Views | Manual queries | Automatic integration |
| Code amount | More code, more control | Less code, more magic |

---

## Requirements

- Drupal 7.x
- PHP 5.6+ (PHP 7.4 recommended)
- MySQL 5.5+
- Entity API module (drupal.org/project/entity)

---

## Installation

1. Copy `medremind/` to `sites/all/modules/custom/`
2. Download and enable Entity API module
3. Enable MedRemind at Admin → Modules
4. Place sidebar blocks at Admin → Structure → Blocks
5. Configure at Admin → Configuration → MedRemind Settings
6. Set front page to `medremind` at Admin → Configuration → Site Information
7. Add menu links: Admin → Structure → Menus → Main menu

---

## Testing

```bash
# Run all tests via command line
php scripts/run-tests.sh --group "MedRemind"

# Or via UI
Admin → Configuration → Development → Testing → Check "MedRemind" → Run tests
```

### Test Coverage

| Test Class | Tests | Coverage |
|------------|-------|----------|
| MedRemindInstallTestCase | 3 | Tables, variables, permissions |
| MedRemindAccessTestCase | 6 | Role-based access (403/200) |
| MedRemindMedicationCRUDTestCase | 9 | Add, edit, delete, validation |
| MedRemindDashboardTestCase | 8 | All pages load correctly |
| MedRemindAjaxTestCase | 5 | Take/skip, streaks, security |
| MedRemindAdminTestCase | 2 | Settings save, statistics |
| MedRemindPrescriptionTestCase | 4 | Entity CRUD, table exists |
| **Total** | **37** | |

---

## Screenshots



---

## Tech Stack

- **CMS:** Drupal 7
- **Language:** PHP 7.4
- **Database:** MySQL 5.7
- **Frontend:** jQuery, CSS3 Grid/Flexbox
- **Architecture:** MVC (Drupal hooks), Entity API, Form API, Theme API
- **Testing:** SimpleTest (DrupalWebTestCase)
- **Version Control:** Git

---

## Author

Built by **Hona** as a portfolio project demonstrating Drupal 7 custom module development — database design (8 tables), Entity API, Form API with validation, AJAX integration, cron automation, testing methodology, and responsive CSS design.

---

## License
