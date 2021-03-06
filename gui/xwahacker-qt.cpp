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

#include <QApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>

#include "xwahacker-qt.h"

#define GUI 1
#include "../xwahacker.c"

static void addResHeading(QGridLayout *grid)
{
    grid->addWidget(new QLabel(QObject::tr("Original")), 0, 0);
    grid->addWidget(new QLabel(QObject::tr("New resolution")), 0, 1, 1, 2);
    grid->addWidget(new QLabel(QObject::tr("FOV")), 0, 3);
    grid->addWidget(new QLabel(QObject::tr("HUD scale")), 0, 4);
}

static void addResNames(QGridLayout *grid)
{
    grid->addWidget(new QLabel("640x480"), 1, 0);
    grid->addWidget(new QLabel("800x600"), 2, 0);
    grid->addWidget(new QLabel("1152x864"), 3, 0);
    grid->addWidget(new QLabel("1600x1200"), 4, 0);
}

static void addSpinBoxes(QGridLayout *grid, QSpinBox *(&sb)[4][2], QDoubleSpinBox *(dsb)[4][2])
{
    for (int i = 0; i < 4; ++i)
    {
        sb[i][0] = new QSpinBox();
        sb[i][0]->setMinimum(640);
        sb[i][0]->setMaximum(8192);
        grid->addWidget(sb[i][0], i+1, 1);
        sb[i][1] = new QSpinBox();
        sb[i][1]->setMinimum(480);
        sb[i][1]->setMaximum(8192);
        grid->addWidget(sb[i][1], i+1, 2);
        dsb[i][0] = new QDoubleSpinBox();
        dsb[i][0]->setMinimum(10);
        dsb[i][0]->setMaximum(170);
        grid->addWidget(dsb[i][0], i+1, 3);
        dsb[i][1] = new QDoubleSpinBox();
        dsb[i][1]->setMinimum(0.1);
        dsb[i][1]->setMaximum(10);
        dsb[i][1]->setSingleStep(0.1);
        grid->addWidget(dsb[i][1], i+1, 4);
    }
}

static const char *opt_names[NUM_OPTS] = {
    [OPT_FIXED_CLEAR] = "Fix graphical corruption like disappearing objects",
    [OPT_FORCE_800] = "Use resolution marked 800x600 above regardless of in-game settings",
    [OPT_USE_32BIT] = "32 bit rendering, breaks load screens",
    [OPT_NOCD] = "Disable CD checks, play from install path",
    [OPT_NOSTARS] = "Disable starfield background (better performance on Linux)",
    [OPT_MSGLOOP] = "Fix keyboard not working in hangar (Linux/WINE fix)",
};

static const char *showfps_names[] = {
    [SHOWFPS_DISABLED] = "Disabled",
    [SHOWFPS_FPS_ONLY] = "FPS only",
    [SHOWFPS_FPS_SCENESTATS] = "FPS and scene statistics",
    [SHOWFPS_FPS_TEXSTATS] = "FPS and scene statistics",
};

XWAHacker::XWAHacker()
{
    QGridLayout *res_layout = new QGridLayout();
    addResHeading(res_layout);
    addResNames(res_layout);
    addSpinBoxes(res_layout, res_spinboxes, fov_hud_spinboxes);
    for (int i = 0; i < 4; ++i)
    {
        res_reset_buttons[i] = new QPushButton(tr("Reset"));
        res_layout->addWidget(res_reset_buttons[i], i + 1, 5);
    }

    QGroupBox *res_group = new QGroupBox(tr("Resolutions"));
    res_group->setLayout(res_layout);

    QGridLayout *opt_layout = new QGridLayout();
    for (int i = 0; i < NUM_OPTS; ++i)
    {
        opts[i] = new QCheckBox(tr(opt_names[i]));
        opt_layout->addWidget(opts[i]);
    }

    QGroupBox *opt_group = new QGroupBox(tr("Options - all disabled in original game"));
    opt_group->setLayout(opt_layout);

    QHBoxLayout *fps_layout = new QHBoxLayout();
    for (int i = 0; i < NUM_SHOWFPS; ++i)
    {
        showfps[i] = new QRadioButton(tr(showfps_names[i]));
        fps_layout->addWidget(showfps[i]);
    }
    QGroupBox *fps_group = new QGroupBox(tr("Show FPS"));
    fps_group->setLayout(fps_layout);

    QHBoxLayout *button_layout = new QHBoxLayout();
    QPushButton *exit = new QPushButton(tr("Exit"));
    button_layout->addWidget(exit);
    QPushButton *save = new QPushButton(tr("Save and exit"));
    button_layout->addWidget(save);

    QVBoxLayout *main_layout = new QVBoxLayout();
    main_layout->addWidget(res_group);
    main_layout->addWidget(opt_group);
    main_layout->addWidget(fps_group);
    main_layout->addLayout(button_layout);

    QWidget *central_widget = new QWidget();
    central_widget->setLayout(main_layout);
    setCentralWidget(central_widget);

    connect(res_spinboxes[0][0], SIGNAL(valueChanged(int)), this, SLOT(res0_change()));
    connect(res_spinboxes[0][1], SIGNAL(valueChanged(int)), this, SLOT(res0_change()));
    connect(res_spinboxes[1][0], SIGNAL(valueChanged(int)), this, SLOT(res1_change()));
    connect(res_spinboxes[1][1], SIGNAL(valueChanged(int)), this, SLOT(res1_change()));
    connect(res_spinboxes[2][0], SIGNAL(valueChanged(int)), this, SLOT(res2_change()));
    connect(res_spinboxes[2][1], SIGNAL(valueChanged(int)), this, SLOT(res2_change()));
    connect(res_spinboxes[3][0], SIGNAL(valueChanged(int)), this, SLOT(res3_change()));
    connect(res_spinboxes[3][1], SIGNAL(valueChanged(int)), this, SLOT(res3_change()));
    for (int i = 0; i < 4; ++i)
    {
        connect(fov_hud_spinboxes[i][0], SIGNAL(valueChanged(double)), this, SLOT(reset_update()));
        connect(fov_hud_spinboxes[i][1], SIGNAL(valueChanged(double)), this, SLOT(reset_update()));
    }

    connect(res_reset_buttons[0], SIGNAL(clicked()), this, SLOT(res0_reset()));
    connect(res_reset_buttons[1], SIGNAL(clicked()), this, SLOT(res1_reset()));
    connect(res_reset_buttons[2], SIGNAL(clicked()), this, SLOT(res2_reset()));
    connect(res_reset_buttons[3], SIGNAL(clicked()), this, SLOT(res3_reset()));

    connect(exit, SIGNAL(clicked()), qApp, SLOT(quit()));
    connect(save, SIGNAL(clicked()), this, SLOT(save()));
}

bool XWAHacker::openBinary(const char *filename)
{
    xwa = fopen(filename, "r+b");
    if (!xwa)
    {
        QMessageBox err;
#ifdef __WIN32__
        err.setText(tr("Could not open file.\nTry running this program as administrator."));
#else
        err.setText(tr("Could not open file"));
#endif
        err.exec();
        return false;
    }

    uint8_t buffer[BUFFER_SZ];
    int count = count_patches(buffer, xwa, binaries[0].patchgroups);
    bool enable_opts = true;
    if (count != num_patchgroups(binaries[0].patchgroups))
    {
        QMessageBox err;
        err.setText(tr(count ? "File has unsupported modifications\nOptions disabled" : "Not a supported XWingAlliance binary"));
        err.exec();
        if (!count)
            return false;
        enable_opts = false;
    }
    for (int i = 0; i < NUM_OPTS; ++i)
    {
        opts[i]->setEnabled(enable_opts);
    }
    for (int i = 0; i < NUM_SHOWFPS; ++i)
    {
        showfps[i]->setEnabled(enable_opts);
    }
    struct resopts resolutions[NUM_RES];
    read_res(buffer, xwa, resolutions);
    for (int i = 0; i < 4; ++i)
    {
        if (resolutions[i].w < 0 || resolutions[i].h < 0 ||
            resolutions[i].fov < 0 || resolutions[i].hud_scale.i == 0xffffffffu)
        {
            QMessageBox err;
            err.setText(tr("Unsupported binary, could not read resolution settings"));
            err.exec();
            return false;
        }
        res_spinboxes[i][0]->setValue(resolutions[i].w);
        res_spinboxes[i][1]->setValue(resolutions[i].h);
        fov_hud_spinboxes[i][0]->setValue(fov2deg(resolutions[i].fov, resolutions[i].h));
        fov_hud_spinboxes[i][1]->setValue(resolutions[i].hud_scale.f);
    }

    for (int i = 0; i < NUM_OPTS; ++i)
    {
        enum PATCHES optsmap[NUM_OPTS] = {
            [OPT_FIXED_CLEAR] = PATCH_CLEAR2,
            [OPT_FORCE_800] = PATCH_FORCE_RES,
            [OPT_USE_32BIT] = PATCH_32BIT_FB,
            [OPT_NOCD] = PATCH_NO_CD_CHECK,
            [OPT_NOSTARS] = PATCH_STARS_OFF,
            [OPT_MSGLOOP] = PATCH_ADD_MSGLOOP,
        };
        opts[i]->setChecked(check_patch(buffer, xwa, optsmap[i], 1));
    }
    if (check_patch(buffer, xwa, PATCH_NO_CD_CHECK2, 1))
    {
        opts[OPT_NOCD]->setChecked(true);
    }

    enum ShowFPS showfps_mode = SHOWFPS_DISABLED;
    if (check_patch(buffer, xwa, PATCH_SHOWFPS_FPS, 1))
    {
        showfps_mode = SHOWFPS_FPS_ONLY;
    }
    else if (check_patch(buffer, xwa, PATCH_SHOWFPS_FPS_SCENESTATS, 1))
    {
        showfps_mode = SHOWFPS_FPS_SCENESTATS;
    }
    else if (check_patch(buffer, xwa, PATCH_SHOWFPS_FPS_TEXSTATS, 1))
    {
        showfps_mode = SHOWFPS_FPS_TEXSTATS;
    }
    for (int i = 0; i < NUM_SHOWFPS; ++i)
    {
        showfps[i]->setChecked(i == showfps_mode);
    }

    reset_update();
    return true;
}

void XWAHacker::res_change(int i)
{
    int h = res_spinboxes[i][1]->value();
    fov_hud_spinboxes[i][0]->setValue(fov2deg(default_fov(h), h));
    fov_hud_spinboxes[i][1]->setValue(default_hud_scale(h));
    reset_update();
}

void XWAHacker::reset_update()
{
    for (int i = 0; i < 4; ++i)
    {
        int h = res_spinboxes[i][1]->value();
        bool enable = res_spinboxes[i][0]->value() != resdes[i].width ||
                      h != resdes[i].height ||
                      deg2fov(fov_hud_spinboxes[i][0]->value(), h) != default_fov(h) ||
                      static_cast<float>(fov_hud_spinboxes[i][1]->value()) != default_hud_scale(h);
        res_reset_buttons[i]->setEnabled(enable);
    }
}

void XWAHacker::res0_change()
{
    res_change(0);
}
void XWAHacker::res1_change()
{
    res_change(1);
}
void XWAHacker::res2_change()
{
    res_change(2);
}
void XWAHacker::res3_change()
{
    res_change(3);
}

void XWAHacker::res_reset(int i)
{
    res_spinboxes[i][0]->setValue(resdes[i].width);
    res_spinboxes[i][1]->setValue(resdes[i].height);
    res_change(i);
}

void XWAHacker::res0_reset()
{
    res_reset(0);
}
void XWAHacker::res1_reset()
{
    res_reset(1);
}
void XWAHacker::res2_reset()
{
    res_reset(2);
}
void XWAHacker::res3_reset()
{
    res_reset(3);
}

void XWAHacker::save()
{
    uint8_t buffer[BUFFER_SZ];
    struct resopts resolutions[NUM_RES];
    read_res(buffer, xwa, resolutions);
    for (int i = 0; i < 4; ++i)
    {
        resolutions[i].w = res_spinboxes[i][0]->value();
        resolutions[i].h = res_spinboxes[i][1]->value();
        int h = resolutions[i].h;
        double deffov = fov2deg(default_fov(h), h);
        double fov = fov_hud_spinboxes[i][0]->value();
        if (fov > deffov - 0.015 && fov < deffov + 0.015)
            resolutions[i].fov = default_fov(h);
        else
            resolutions[i].fov = deg2fov(fov, h);
        float defhud = default_hud_scale(h);
        double hud = fov_hud_spinboxes[i][1]->value();
        if (hud > defhud - 0.015 && hud < defhud + 0.015)
            resolutions[i].hud_scale.f = defhud;
        else
            resolutions[i].hud_scale.f = hud;
        if (!write_res(buffer, xwa, resolutions + i, i, 0, 0))
        {
            QMessageBox err(this);
            err.setText(tr("Failed writing resolution values"));
            err.exec();
            return;
        }
    }
    for (int i = 0; i < NUM_OPTS; ++i)
    {
        if (!opts[i]->isEnabled())
            continue;
        bool c = opts[i]->isChecked();
        int opt2collection[NUM_OPTS][2] = {
            [OPT_FIXED_CLEAR] = {2, 3},
            [OPT_FORCE_800] = {4, 5},
            [OPT_USE_32BIT] = {0, 1},
            [OPT_NOCD] = {6, 7},
            [OPT_NOSTARS] = {-PATCH_STARS_ON, -PATCH_STARS_OFF},
            [OPT_MSGLOOP] = {-PATCH_NO_MSGLOOP, -PATCH_ADD_MSGLOOP},
        };
        int collection = opt2collection[i][c];
        int res = 0;
        if (collection < 0)
        {
            res = apply_patch(buffer, xwa, binaries[0].patchgroups, static_cast<enum PATCHES>(-collection));
        }
        else
        {
            res = apply_collection(buffer, xwa, binaries, collection);
        }
        if (!res)
        {
            QMessageBox err(this);
            err.setText(tr("Failed setting option") + tr(opt_names[i]));
            err.exec();
            return;
        }
    }
    if (showfps[0]->isEnabled())
    {
        int res = 0;
        int collection = -1;
        for (int i = 0; i < NUM_SHOWFPS; ++i)
        {
            if (showfps[i]->isChecked())
            {
                if (collection != -1)
                {
                    collection = -2;
                    break;
                }
                collection = 8 + i;
            }
        }
        if (collection >= 0)
        {
            res = apply_collection(buffer, xwa, binaries, collection);
        }
        if (!res)
        {
            QMessageBox err(this);
            err.setText(tr("Failed setting FPS display mode"));
            err.exec();
            return;
        }
    }
    QMessageBox done(this);
    done.setText(tr("Changes saved successfully!"));
    done.exec();

    qApp->quit();
}
