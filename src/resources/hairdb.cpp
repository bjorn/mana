/*
 *  Color database
 *  Copyright (C) 2008  Aethyra Development Team
 *  Copyright (C) 2009-2012  The Mana Developers
 *
 *  This file is part of The Mana Client.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "resources/hairdb.h"

#include "log.h"

#include "utils/xml.h"

#include <libxml/tree.h>

namespace
{
    HairDB::Colors mColors;
    bool mLoaded = false;
    std::string mFail = "#ffffff";
}

void HairDB::load()
{
    if (mLoaded)
        unload();

    XML::Document *doc = new XML::Document(HAIR_XML_FILE);
    xmlNodePtr root = doc->rootNode();
    bool hairXml = true;

    if (!root || !xmlStrEqual(root->name, BAD_CAST "colors"))
    {
        logger->log("Trying to fall back on " COLORS_XML_FILE);

        hairXml = false;

        delete doc;
        doc = new XML::Document(COLORS_XML_FILE);
        root = doc->rootNode();

        if (!root || !xmlStrEqual(root->name, BAD_CAST "colors"))
        {
            logger->log("ColorDB: Failed to find any color files.");
            mColors[0] = mFail;
            mLoaded = true;

            delete doc;

            return;
        }
    }
    for_each_xml_child_node(node, root)
    {
        if (xmlStrEqual(node->name, BAD_CAST "color"))
        {
            int id = XML::getProperty(node, "id", 0);

            if (mColors.find(id) != mColors.end())
            {
                logger->log("ColorDB: Redefinition of dye ID %d", id);
            }

            mColors[id] = hairXml ? XML::getProperty(node, "value", "#FFFFFF") :
                                    XML::getProperty(node, "dye", "#FFFFFF");
        }
    }

    delete doc;

    mLoaded = true;
}

void HairDB::unload()
{
    logger->log("Unloading color database...");

    mColors.clear();
    mLoaded = false;
}

std::string &HairDB::get(int id)
{
    if (!mLoaded)
        load();

    ColorIterator i = mColors.find(id);

    if (i == mColors.end())
    {
        logger->log("ColorDB: Error, unknown dye Id# %d", id);
        return mFail;
    }
    else
    {
        return i->second;
    }
}

int HairDB::size()
{
    return mColors.size();
}