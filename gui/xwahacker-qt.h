/*
 * Qt GUI for XWAHacker
 * Copyright (C) 2015 Reimar Döffinger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef XWAHACKER_QT_H
#define XWAHACKER_QT_H

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QMainWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

enum {
    OPT_FIXED_CLEAR = 0,
    OPT_FORCE_800,
    OPT_USE_32BIT,
    OPT_NOCD,
    OPT_NOSTARS,
    OPT_MSGLOOP,
    NUM_OPTS
};

enum ShowFPS {
    SHOWFPS_DISABLED = 0,
    SHOWFPS_FPS_ONLY,
    SHOWFPS_FPS_SCENESTATS,
    SHOWFPS_FPS_TEXSTATS,
    NUM_SHOWFPS
};

class XWAHacker : public QMainWindow
{
    Q_OBJECT

public:
    XWAHacker();
    virtual ~XWAHacker() {}
    bool openBinary(const char *filename);

private slots:
    void res0_change();
    void res1_change();
    void res2_change();
    void res3_change();
    void res0_reset();
    void res1_reset();
    void res2_reset();
    void res3_reset();
    void reset_update();
    void save();

private:
    void res_change(int i);
    void res_reset(int i);
    QSpinBox *res_spinboxes[4][2];
    QDoubleSpinBox *fov_hud_spinboxes[4][2];
    QPushButton *res_reset_buttons[4];
    QCheckBox *opts[NUM_OPTS];
    QRadioButton *showfps[NUM_SHOWFPS];
    FILE *xwa;
};

#endif
