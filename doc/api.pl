#!/usr/bin/perl
use IO::File;

# Open up the output files
$fh = new IO::File "> api.html";

# Lists for the index
@IText = ();
@ISettings = ();
@IDrawing = ();
@IDevices = ();
@IInteractive = ();

@Device = ();
@Test = ();
@Settings = ();
@Drawing = ();
@Interactive = ();

# Subroutine for parsing individual comments
sub processComment
{
  my $fh;
  my @prototype;
  my $type = '';
  my $name = '';
  my $desciption = '';
  my @seeAlso;
  my @lists;

  $desciption = '';
  undef @lists;
  undef @seeAlso;
  undef @prototype;
  # Get all the info
  do
  {
    # Get the name and type
    if ($_[0] =~ m/.* (Interactive|Text|Drawing|Device|Settings):\s*(.*)$/)
    {
      ($type, $name) = ($1, $2);
    }
    # Get the desciption
    elsif ($_[0] =~ m/^.*Synopsis:\s*(.*)$/)
    {
      $description = $1;
      while ($_[1] =~ m/\s\*\s*(\S.*)$/)
      {
	$description = $description . ' ' . $1;
	shift;
      }
    }
    # Get the see also list
    elsif ($_[0] =~ m/.* See Also: (.*)$/)
    {
      @seeAlso = split (/,\s*/, $1);
    }
    # Get any lists
    elsif ($_[0] =~ m/\s*\*\s*(\S.*):/)
    {
      # the title
      my @tmp = ();
      push @tmp, $1;

      # the items
      while ($_[1] =~ m/\s*\*\s*\-(.*)\s*:\-\s*(\S.*)$/)
      {
	push @tmp, $1;
	push @tmp, $2;
	shift;

	# Items over more than one line.
	while ($_[1] !~ m/^\s*\*\s*\-.*\s*:\-\s*\S.*$/ && $_[1] !~ m/^\s*\*\s*$/ && $_[1] =~ m/^\s*\*/)
	{
	  $_[1] =~ m/^\s*\*\s*(\S.*)$/;
	  push (@tmp, pop (@tmp) . ' ' . $1);
	  shift;
	}
      }
      push @lists, \@tmp;
    }
    # Fuction prototype
    elsif ($_[0] !~ m/^ \*/)
    {
      # Type
      chomp ($_[0]);
      push @prototype, $_[0];
      shift;
      chomp ($_[0]);
      # Name
      push @prototype, substr ($_[0], 0, index ($_[0], ' '));
      # Arguments
      push @prototype, substr ($_[0], index ($_[0], ' '));
      while (shift) 
      {
	$_[0] =~ m/^\s*(.*)$/;
	push @prototype, pop (@prototype) . " $1";
      }
    }
  } while (shift);

  # Add to the index and description lists 
  if ($type eq "Text")
  {
    $IFile = \@IText;
    $File = \@Text;
  }
  elsif ($type eq "Drawing")
  {
    $IFile = \@IDrawing;
    $File = \@Drawing;
  }
  elsif ($type eq "Device")
  {
    $IFile = \@IDevice;
    $File = \@Device;
  }
  elsif ($type eq "Settings")
  {
    $IFile = \@ISettings;
    $File = \@Settings;
  }
  elsif ($type eq "Interactive")
  {
    $IFile = \@IInteractive;
    $File = \@Interactive;
  }
  else
  {
    return;
  }

  push @$IFile, $name;

  # Name
  push @$File, "<h3><a name=\"$name\">$name</a><hr></h3>\n";
  # Prototype
  push @$File, "<table class=\"proto\">\n<tr><td width=10%>" . shift (@prototype) . "</td><td width=20%>" . shift (@prototype) . "</td><td>" . shift (@prototype) . ";</tr></table>";
  # Description
  push @$File, "<p>$description</p>\n";
  # Lists
  foreach $item (@lists)
  {
    # Name
    push @$File, "<h4>" . shift (@$item) . ":</h4>\n";
    # Table preamble
    push @$File, "<table class=\"api\">";
    # Table contents
    while (@$item)
    {
      push @$File, "<tr>\n<td>" . shift (@$item) . ":</td>";
      push @$File, "<td>" . shift (@$item) . "</td>\n</tr>";
    }
    # Table postamble
    push @$File, "</table>";
  }
  # See Also
  if (@seeAlso)
  {
    push @$File, "<h4>See Also:</h4><p>";
    foreach (@seeAlso)
    {
      push @$File, "<a href=\"#$_\">$_</a> ";
    }
    push @$File, "</p>";
  } 

}

# prossess each comment
my @currentComment = ();
my $inComment = 0;
while (<>)
{
  # Begin a comment
  if ($_ =~ m/\/\*\*/ && $inComment == 0)
  {
    $inComment = 1;
  }
  # End comment
  elsif ($_ =~ m/\*\// && $inComment == 1)
  {
    # Get the prototype as well
    my $nextL = <>;
    while ($nextL && $nextL !~ m/{/)
    {
      push @currentComment, $nextL;
      $nextL = <>;
    }
    &processComment (@currentComment);
    @currentComment = ();
    $inComment = 0;
  }
  # Add to comment
  elsif ($inComment == 1)
  {
    push (@currentComment, $_);
  }
}

# Write the output file
# First the formating junk
$fh->print (
"<html>
  <head>
    <meta http-equiv=\"Content-Type\" content=\"text/html\"; charset=utf-8\" \>
    <title>giza - API Reference Manual</title>
    <link rel=\"stylesheet\" href=\"../style.css\" type=\"text/css\" \>
  </head>

  <div class=\"header\">
    <a href=\"/\"><img alt=\"giza\" src=\"../title.png\"\></a>
  </div>

  <div id=\"menucontainer\">
  <ul id=\"menulist\">
    <li><a href=\"/news/\">news</a></li>
    <li><a href=\"/download/\">download</a></li>
    <li><a href=\"/documentation/\">documentation</a></li>
    <li><a href=\"/contact/\">contact</a></li>
    <li><a href=\"/samples/\">samples</a></li>
  </ul>
  </div>

  <div id=\"content\">
  <h1>API Reference Manual</h1>
  <p>Here we provided a function by function reference for the low-level C API to giza. 
  <br/>Please note that the giza API is <strong>not yet stable</strong>.
  From version 1.0 we will aim to keep backwards compatibility, but in the 0.x series, changes to the API may be made arbitrarily.</p>
  <h1>Index</h1>
  <dl class=\"toc\">");
# Then the index
$fh->print ("<dt><a href=\"#Device_management\">Device Management</a></dt>\n");
foreach (@IDevice)
{
  $fh->print ("<dd><a href=\"#$_\">$_</a></dd>\n");
}

$fh->print ("<dt><a href=\"#Drawing\">Drawing</a></dt>\n");
foreach (@IDrawing)
{
  $fh->print ("<dd><a href=\"#$_\">$_</a></dd>\n");
}

$fh->print ("<dt><a href=\"#Text\">Text</a></dt>\n");
foreach (@IText)
{
  $fh->print ("<dd><a href=\"#$_\">$_</a></dd>\n");
}

$fh->print ("<dt><a href=\"#Settings\">Settings</a></dt>\n");
foreach (@ISettings)
{
  $fh->print ("<dd><a href=\"#$_\">$_</a></dd>\n");
}
$fh->print ("<dt><a href=\"#Interactive\">Interactive</a></dt>\n");
foreach (@IInteractive)
{
  $fh->print ("<dd><a href=\"#$_\">$_</a></dd>\n");
}
$fh->print ("</dl>");

# Write descriptions for Device Management
$fh->print ("<a name=\"Device_managment\"></a><h1>Device Managment</h1>\n");
foreach (@Device)
{
  $fh->print ("$_");
}

# Write descriptions for Drawing
$fh->print ("<a name=\"Drawing\"></a><h1>Drawing</h1>\n");
foreach (@Drawing)
{
  $fh->print ("$_");
}

# Write descriptions for Text
$fh->print ("<a name=\"Text\"></a><h1>Text</h1>\n");
foreach (@Text)
{
  $fh->print ("$_");
}

# Write descriptions for Settings
$fh->print ("<a name=\"Settings\"></a><h1>Settings</h1>\n");
foreach (@Settings)
{
  $fh->print ("$_");
}

# Write descriptions for Interactive
$fh->print ("<a name=\"Interactive\"></a><h1>Interactive</h1>\n");
foreach (@Interactive)
{
  $fh->print ("$_");
}

# Close the files
$fh->flush ();
$fh->close ();
