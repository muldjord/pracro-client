/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            strtools.cpp
 *
 *  Wed Jun 7 12:00:00 CEST 2017
 *  Copyright 2017 Lars Bisballe
 *  larsbisballe@gmail.com
 ****************************************************************************/
/*
 *  This file is part of Pracro.
 *
 *  Pracro is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Pracro is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Pracro; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#include <QRegularExpression>

#include "strtools.h"

bool StrTools::isPatientId(const QString patientId)
{
  if(QRegularExpression("^[0-3]{1}[0-9]{1}[0-1]{1}[0-9]{1}[0-9]{2}-[a-zA-Z0-9]{4}$").match(patientId).hasMatch()) {
    return true;
  }
  return false;
}
