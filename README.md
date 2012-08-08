Lib Hours Utexas for Drupal 6.x
-------------------------------

author Dustin Younse <dyounse@austin.utexas.edu>

This is a heavily modified version of Sean Watkins' Libhours-2.x module, whose original README is pasted below.

Multiple changes were made to better suit our needs, which are enumerated below.

* On a purely presentational level, removed the use of the libhours-location-arrow.png file due to several of our locations having long names that exceeded the dimensions of this file, and changed the base colors to reflect university colors.

* Modified many dimensions in the .css file from px to percentages to allow a more flexible layout.

* Modified the .module to sort the list of locations by LID rather than Name, allowing our flagship branch, the Perry-Castañeda Library, to appear at the top of the list without making db modifications.

* Modified the .module time pre-process function to display Noon and Midnight when opening or closing times equaled 1200 or 0, resepectively, for clarity.

* Modified libhours_type db table to have only two exceptions, Planned and Emergency.  Modified the .module file to have two Exceptions output loops, one for emergencies and one for planned.  This allowed for theming hooks to be inserted into the Emergency exceptions.  This modification also involved the .tpl.php and .css files.

- Emergency exceptions MUST be of TID 2 to work properly with this modified .module file.

* Modified the .install file to reflect our needed Period names in the libhours_semesters table.

Lib Hours 2.x for Drupal 6.x
----------------------------

author Sean Watkins <slwatkins@uh.edu>
copyright 2011 University of Houston Libraries (http://info.lib.uh.edu)


INSTALL
=======

Installing Lib Hours is like any other Drupal module. Simply place the module directory
in your sites modules folder and enable it from 'admin/build/modules'.

Lib Hours will then need to be configured with locations and periods to add hours. Go to
'admin/content/libhours' page to configure.

All colors and stylings can be changed in the libhours.css file located in the module 
directory.

Hours page can be accessed at 'hours/' and will default to the first location entered 
into the system.

API
===

Lib Hours comes with a REST API to allow integration with other applications.

ENDPOINT: /hours/api

The following paramaters are required and must be used in order to make a successful call.

PARAMATER		VALUES
~~~~~~~~~		~~~~~~
format			json
action			locations, hours, open

The following paramaters are required when the action 'hours' is used.

PARAMATER		VALUES
~~~~~~~~~		~~~~~~
library			integer, library unique identifier

The following paramaters are optional.

PARAMATER		VALUES
~~~~~~~~~		~~~~~~
callback		Set the callback to return in the results for jsonp compliance.


EXAMPLES
--------

Example: Returns a json response listing all library locations and their unique 
identifiers.

http://example.com/hours/api?format=json&action=locations

Result:
{
	status: "ok",
	locations: [
		{
			id: "2",
			location: "Architecture & Art Library"
		},
		{
			id: "1",
			location: "M.D. Anderson Library"
		},
		{
			id: "5",
			location: " -- Special Collections"
		},
		{
			id: "3",
			location: "Music Library"
		},
		{
			id: "4",
			location: "Optometry Library"
		}
	]
}

Example: Returns hours and hour exceptions for the current period for the given library 
unique identifier.

http://example.com/hours/api?format=json&action=hours&library=1

Result:
{
	status: "ok",
	location: "M.D. Anderson Library",
	period: "Intersession",
	hours: [
		{
			dayofweek: "Sunday",
			hour: "1:00pm - 4:45pm"
		},
		{
			dayofweek: "Monday",
			hour: "7:00am - 5:45pm"
		},
		{
			dayofweek: "Tuesday",
			hour: "7:00am - 5:45pm"
		},
		{
			dayofweek: "Wednesday",
			hour: "7:00am - 7:45pm"
		},
		{
			dayofweek: "Thursday",
			hour: "7:00am - 5:45pm"
		},
		{
			dayofweek: "Friday",
			hour: "7:00am - 5:45pm"
		},
		{
			dayofweek: "Saturday",
			hour: "10:00am - 4:45pm"
		}
	],
	exceptions: [
		{
			date: "1/16 Monday",
			hour: "Closed"
		}
	]
}

Example: Returns list of locations currently open.

http://example.com/hours/api?format=json&action=open

{
	status: "ok",
	locations: [
		{
			id: "1",
			location: "M.D. Anderson Library",
			hours: "7:00am - 7:45pm"
		},
		{
			id: "4",
			location: "Optometry Library",
			hours: "7:30am - 9:00pm"
		}
	]
}