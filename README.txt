// $Id$

Date Repeat Formatter
------------------------

This Drupal module provides two formatters for CCK date fields (type 'date', 
'datetime' and 'datestamp'). Its primary purpose is to tidy up and make  
more themeable the output when using repeating date functionality via the 
Date Repeat API.

Formatters:

1.  Date Formatter

    Variable output of a simple dateline.
    
    Examples (default output):
  
    A)  Sunday, January 2, 2011 - 14:00-16:00
        Standard date output.
    
    B)  Mon, 2011-02-21 01:00 (ended)
        Standard date output, date has passed.

    C)  Next event (Daily): Tue, 2011-03-01 02:00
        If a date with repeating frequency, will display the next event,
        with frequency, if it exists. If repeating in intervals will
        display (e.g.) "(Every 3 weeks)".
        
    D)  (Daily event) February 01, 2011 to February 09, 2011 (ended)
        A repeating date that has ended will display the repeat
        rule "(Daily event)" as well as the time period during which
        the event repeated.

2)  Date Formatter - Expanded

    Same as above, but appends a list of all repeating dates, hidden by 
    default, with a toggle view/hide link. This is the recommended formatter 
    for full node view

    Next event (Daily): Tue, 2011-03-01 02:00 | View all dates |
    -------- toggle show/hide on dates below ------> 
    All scheduled dates:
      Tue, 2011-03-01 02:00
      Wed, 2011-03-02 02:00 (ended)
      Thu, 2011-03-03 02:00
      Fri, 2011-03-04 02:00

INSTALLATION
------------
     
Installing modules and themes:
http://drupal.org/documentation/install/modules-themes

SET-UP
------------

These instructions assume you have created a CCK date field for
a content type.

1.  Go to the "Display fields" page for your content type.
    e.g. /admin/content/node-type/[type-name]/display

2.  Choose a formatter from the dropdown in the "Teaser" and/or "Full" columns. Either
    "Date Formatter" or "Date Formatter - Expanded". 
    
3.  If your field has repeating dates enabled, you should also see an option to select
    whether to "Display From and To dates", "Display From date only", or "Display To
    date only". Your selection here will apply.
    
3.  Hit "Save". That's it!

THEMING
------------

To make the output accessible to themers, there are several theme functions which 
handle different "chunks" of output, such as the full listing of repeating dates,
the repeat rule and the "(ended)" string. 

See the theming guide for more info on theming with Drupal:
http://drupal.org/documentation/theme

DEVELOPERS
------------

The formatters use field information contained in objects that are created on node  
view, after which these stored objects are accessible by calling:

  $date = date_repeat_fm_fetch($nid, $field_name);

To create a new object:

  $date = new fDate($node, $field_name);
  

