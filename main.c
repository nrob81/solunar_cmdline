/*=======================================================================
solunar
main.c
Copyright (c)2005-2019 Kevin Boone
=======================================================================*/
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "city.h"
#include "cityinfo.h"
#include "latlong.h"
#include "pointerlist.h"
#include "error.h"
#include "datetime.h"
#include "suntimes.h"
#include "moontimes.h"
#include "holidays.h"
#include "astrodays.h"
#include "nameddays.h"
#include "solunar.h"


/*=======================================================================
print_sunrise_time
=======================================================================*/
void print_sunrise_time (char *text, double zenith, BOOL opt_utc, 
   const char *tz, const LatLong *latlong, const DateTime *date, 
   BOOL twelve_hour, BOOL opt_syslocal)
  {
  char *s;
  Error *e = NULL;
  DateTime *sunrise = SunTimes_get_sunrise (latlong, date, 
      zenith, tz, &e);
  if (e)
    {
    s = strdup (Error_get_message (e));
    Error_free (e);
    }
  else
    {
    if (opt_syslocal)
      s = DateTime_time_to_string_syslocal (sunrise, twelve_hour);
    else if (opt_utc)
      s = DateTime_time_to_string_UTC (sunrise, twelve_hour);
    else
      s = DateTime_time_to_string_local (sunrise, tz, twelve_hour);
    DateTime_free (sunrise);
    }
  printf ("%s%s\n", text, s);
  free (s);
  }


/*=======================================================================
print_sunset_time
=======================================================================*/
void print_sunset_time (char *text, double zenith, BOOL opt_utc, const char *tz,
    const LatLong *latlong, const DateTime *date, BOOL twelve_hour, 
    BOOL opt_syslocal)
  {
  char *s;
  Error *e = NULL;
  DateTime *sunrise = SunTimes_get_sunset (latlong, date, 
      zenith, tz, &e);
  if (e)
    {
    s = strdup (Error_get_message (e));
    Error_free (e);
    }
  else
    {
    if (opt_syslocal)
      s = DateTime_time_to_string_syslocal (sunrise, twelve_hour);
    else if (opt_utc)
      s = DateTime_time_to_string_UTC (sunrise, twelve_hour);
    else
      s = DateTime_time_to_string_local (sunrise, tz, twelve_hour);
    DateTime_free (sunrise);
    }
  printf ("%s%s\n", text, s);
  free (s);
  }


/*=======================================================================
print_high_noon_time
=======================================================================*/
void print_high_noon_time (char *text, BOOL opt_utc, const char *tz,
    const LatLong *latlong, const DateTime *date, BOOL twelve_hour, 
    BOOL opt_syslocal)
  {
  char *s;
  Error *e = NULL;
  DateTime *sunrise = SunTimes_get_high_noon (latlong, date, tz, &e);
  if (e)
    {
    s = strdup (Error_get_message (e));
    Error_free (e);
    }
  else
    {
    if (opt_syslocal)
      s = DateTime_time_to_string_syslocal (sunrise, twelve_hour);
    else if (opt_utc)
      s = DateTime_time_to_string_UTC (sunrise, twelve_hour);
    else
      s = DateTime_time_to_string_local (sunrise, tz, twelve_hour);
    DateTime_free (sunrise);
    }
  printf ("%s%s\n", text, s);
  free (s);
  }


/*=======================================================================
get_named_days_today
=======================================================================*/
PointerList *initialize_day_events (const char *tz, BOOL utc, int year, 
    const LatLong *latlong)
  {
  BOOL southern = FALSE;
  if (latlong)
    {
    if (LatLong_get_latitude (latlong) < 0)
      southern = TRUE;
    }
  return NamedDays_get_list_for_year (year, tz, utc, southern);
  }

/*=======================================================================
get_named_days_today
=======================================================================*/
PointerList *get_named_days_today (PointerList *day_events, DateTime *day)
  {
  PointerList *list = NULL;
  int i, l = PointerList_get_length (day_events);
  for (i = 0; i < l; i++)
    {
    DateTime *event = PointerList_get_pointer (day_events, i);
    if (DateTime_is_same_day (day, event))
      {
      DateTime *event2 = DateTime_clone (event); 
      list = PointerList_append (list, event2);
      }
    }
  return list;
  }

/*=======================================================================
free_day_events
=======================================================================*/
void free_day_events (PointerList *events)
  {
  int i, l = PointerList_get_length (events);
  for (i = 0; i < l; i++)
    {
    DateTime_free ((DateTime *) PointerList_get_pointer (events, i));
    }
  PointerList_free (events, FALSE);
  }


/*=======================================================================
list_named_days
=======================================================================*/
void list_named_days (PointerList *day_events, const DateTime *start, 
     const char *tz, BOOL utc)
  {
  DateTime *d = DateTime_clone (start);
  int i;
  for (i = 0; i < 365; /* leap year? */ i++)
    {
    int i, l = PointerList_get_length (day_events);
    for (i = 0; i < l; i++)
      {
      DateTime *event = PointerList_get_pointer (day_events, i);
      if (DateTime_is_same_day_of_year (d, event))
        {
        int j;
        const char *name = DateTime_get_name (event);
        char *s = DateTime_date_to_string_syslocal (event);
        printf ("%s", s);
        putchar (' ');
        for (j = strlen (s); j < 26; j++) 
          putchar (' ');
        printf ("%s", name);
        int dummy, hour, min, sec;
        DateTime_get_ymdhms (event, &dummy, &dummy, &dummy, &hour,
          &min, &sec, tz, utc);
        if (hour == 0 && min == 0 && sec == 0)
          {
          // All day event
          }
        else
          {
          printf (" (%02d:%02d)", hour, min);
          }
        putchar ('\n');
        free (s);
        }
      }
    DateTime_add_days (d, 1, tz, utc); 
    } 
  DateTime_free (d);
  }


/*=======================================================================
get_stars
=======================================================================*/
const char *get_stars (double score)
  {
  static char s [20];
  s[10] = 0;
  int i;
  for (i = 0; i < 10; i++)
    {
    if (score > (double) i / 10.0)
      s[i] = '*';
    else
      s[i] = ' ';
    }
  return s;
  }

/*=======================================================================
process
=======================================================================*/
int process (char *city, char *latlong, char *datetime, BOOL opt_utc, BOOL opt_syslocal, BOOL opt_list_named_days, BOOL opt_show_solunar) {
  BOOL opt_twelvehour = FALSE;

  BOOL show_sunrise_sunset = TRUE;
  BOOL show_moon_state = TRUE;
  BOOL show_moon_rise_set = TRUE;
  BOOL show_today = TRUE;
  char *tz = NULL;
  DateTime *datetimeObj = NULL;
  City *cityObj = NULL;
  LatLong *latlongObj = NULL;
  LatLong *workingLatlong = NULL;
  PointerList *day_events = NULL;
  
  if (opt_syslocal && opt_utc) {
    fprintf (stderr, "'utc' and 'syslocal' cannot be specified at the same time;\n");
    fprintf (stderr, "'%s --longhelp' for usage\n", "argv[0]");
    exit (-1);
  }
  
  if (city) {
    PointerList *cities = City_get_matching_name (city);
    int i, l = PointerList_get_length (cities);

    if (l == 0) {
      fprintf (stderr, "No city matching \"%s\"\n\"%s --cities\" shows a list\n", city, "argv[0]");
      PointerList_free (cities, TRUE);
      exit (-1);
    }

    if (l > 1) {
      fprintf (stderr, "Ambiguous city \"%s\". Matches:\n", city);
      for (i = 0; i < l; i++) {
        const char *name = PointerList_get_const_pointer (cities, i);
        printf ("  %s\n", name);
      }
      PointerList_free (cities, TRUE);
      exit (0);
    }

    // If we get here, we have been told exactly one city. 
    cityObj = City_new_from_name (PointerList_get_const_pointer (cities, 0));
    PointerList_free (cities, TRUE);
  }

  if (latlong) {
    Error *error = NULL;
    latlongObj = LatLong_new_parse (latlong, &error);
    if (error) {
      fprintf (stderr, "%s", Error_get_message (error));
      fprintf (stderr, "\n\"%s --latlong help\" for syntax.\n", "argv[0]");
      Error_free (error);
      exit (-1);
    }
  }

  if (cityObj) {
    printf ("Selected city %s\n", cityObj->name);
    tz = cityObj->name;
  }

  if (latlongObj) {
    workingLatlong = LatLong_clone (latlongObj);
    if (cityObj) {
      printf ("Overriding city location with specified lat/long\n");
    } else {
      // No city specified -- use lat/long but don't display warning
    }
  } else {
    if (cityObj) {
      workingLatlong = City_get_latlong (cityObj);
    } else {
      // ...else we have no location
    }
  }

  if (workingLatlong) {
    char *s = LatLong_to_string (workingLatlong);
    printf ("Using location %s\n", s);
    free (s);
  }


  if (datetime) {
    Error *e = NULL;
    datetimeObj = DateTime_new_parse (datetime, &e, tz, opt_utc);
    if (e) {
      fprintf (stderr, "%s", Error_get_message (e));
      fprintf (stderr, "\n\"%s --datetime help\" for syntax.\n", "argv[0]");
      Error_free (e);
      free_day_events (day_events);
      exit (-1);
    }
  } else {
    datetimeObj = DateTime_new_today ();
  }

  if (show_today) {
    int year, dummy;
    DateTime_get_ymdhms (datetimeObj, &year, &dummy, &dummy, &dummy, &dummy, &dummy, tz, opt_utc);
    day_events = initialize_day_events (tz, opt_utc, year, workingLatlong);
  }


  char *s;
  if (opt_utc) {
    s = DateTime_to_string_UTC (datetimeObj);
    printf ("Date/time %s %s\n", s, "UTC");
  } else if (opt_syslocal || !tz) {
    s = DateTime_to_string_syslocal (datetimeObj);
    printf ("Date/time %s %s\n", s, "syslocal");
  } else {
    s = DateTime_to_string_local (datetimeObj, tz);
    printf ("Date/time %s %s\n", s, "local");
  }
  free (s);
  printf ("\n");

  if (opt_list_named_days) {
    if (!datetimeObj) {
      fprintf (stderr, "Can't list days because no starting date has been specified.\n");
      free_day_events (day_events);
      exit (-1);
    }
    DateTime *jan_first = DateTime_get_jan_first (datetimeObj, tz, opt_utc);
    list_named_days (day_events, jan_first, tz, opt_utc);
    DateTime_free (jan_first);
    exit (0);
  }
 
  DateTime *start = DateTime_get_day_start (datetimeObj, tz);

  if (show_today) {
    if (!datetimeObj) {
      fprintf (stderr, "Can't calculate astronomical dates because no calendar date/time has been specified.\n");
      free_day_events (day_events);
      exit (-1);
    }

    printf ("Today\n");
    char *s;
    if (opt_syslocal)
      s = DateTime_date_to_string_syslocal (start);
    else if (opt_utc)
      s = DateTime_date_to_string_UTC (start);
    else
      s = DateTime_date_to_string_local (start, tz);
    //char *s = DateTime_date_to_string (start);
    printf ("                          Date: %s\n", s);
    free (s);

    PointerList *events = get_named_days_today (day_events, datetimeObj);
    int i, l = PointerList_get_length (events);
    if (l > 0) {
      printf ("                      Today is: ");
      for (i = 0; i < l; i++) {
        DateTime *event = PointerList_get_pointer (events, i);
        const char *name = DateTime_get_name (event);
        if (name) {
          if (i != 0) printf (", ");
          printf ("%s", name);
        }
        DateTime_free (event);
      }
      printf ("\n");
    }
    PointerList_free (events, FALSE);

    printf ("                   Day of year: %d\n", DateTime_get_day_of_year (datetimeObj, tz));
    printf ("                   Julian date: %.2lf\n", DateTime_get_julian_date (datetimeObj));
    printf ("          Modified Julian date: %.2lf\n", DateTime_get_modified_julian_date (datetimeObj));
    printf ("\n");
  } 

  BOOL twelve_hour = opt_twelvehour;

  if (show_sunrise_sunset) {
    if (!workingLatlong) {
      fprintf (stderr, "Can't calculate sunrise/set times because no location has been specified.\n");
      free_day_events (day_events);
      exit (-1);
    }
    if (!datetimeObj) {
      fprintf (stderr, "Can't calculate sunrise/set times because no date has been specified.\n");
      exit (-1);
    }
    printf ("Sun\n");
    print_sunrise_time ("                       Sunrise: ", SUNTIMES_DEFAULT_ZENITH, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 
    print_sunset_time ("                        Sunset: ", SUNTIMES_DEFAULT_ZENITH, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 
    print_high_noon_time ("                     High noon: ", opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 

    print_sunrise_time ("         Civil twilight starts: ", SUNTIMES_CIVIL_TWILIGHT, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 
    print_sunset_time ("           Civil twilight ends: ", SUNTIMES_NAUTICAL_TWILIGHT, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 

    print_sunrise_time ("      Nautical twilight starts: ", SUNTIMES_NAUTICAL_TWILIGHT, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 
    print_sunset_time ("        Nautical twilight ends: ", SUNTIMES_NAUTICAL_TWILIGHT, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 
    print_sunrise_time ("  Astronomical twilight starts: ", SUNTIMES_ASTRONOMICAL_TWILIGHT, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 
    print_sunset_time ("    Astronomical twilight ends: ", SUNTIMES_ASTRONOMICAL_TWILIGHT, opt_utc, tz, workingLatlong, datetimeObj, twelve_hour, opt_syslocal); 
    printf ("\n");
  }

  if (show_moon_state || TRUE) {
    if (!datetimeObj) {
      fprintf (stderr, "Can't calculate moon phase because no date has been specified.\n");
      free_day_events (day_events);
      exit (-1);
    }
    printf ("Moon\n");
    if (show_moon_state) {
      double phase, age, distance;
      MoonTimes_get_moon_state (datetimeObj, &phase, &age, &distance); 
      const char *phase_name = MoonTimes_get_phase_name (phase);
      printf ("                    Moon phase: %.2lf %s\n", phase, phase_name);
      printf ("                      Moon age: %.1lf days\n", age);
      printf ("                 Moon distance: %.lf km\n", distance);
    }
    if (show_moon_rise_set) {
      if (!workingLatlong) {
        fprintf (stderr, "Can't calculate sunrise/set times because no location has been specified.\n");
        free_day_events (day_events);
        exit (-1);
      }
      int i, nevents = 0;

      DateTime *end = DateTime_get_day_end (datetimeObj, tz);

      DateTime *events[4];
      MoonTimes_get_moon_rises (workingLatlong, start, end, 15 * 60, events, 4, &nevents);
      for (i = 0; i < nevents; i++) {
        char *s;
        if (opt_syslocal)
          s = DateTime_time_to_string_syslocal (events[i], twelve_hour);
        else if (opt_utc)
          s = DateTime_time_to_string_UTC (events[i], twelve_hour);
        else
          s = DateTime_time_to_string_local (events[i], tz, twelve_hour);
        printf ("                      Moonrise: %s\n", s);
        free (s);
        DateTime_free (events[i]);
      }
      MoonTimes_get_moon_sets (workingLatlong, start, end, 15 * 60, events, 4, &nevents);
      for (i = 0; i < nevents; i++) {
        char *s;
        if (opt_syslocal)
          s = DateTime_time_to_string_syslocal (events[i], twelve_hour);
        else if (opt_utc)
          s = DateTime_time_to_string_UTC (events[i], twelve_hour);
        else
          s = DateTime_time_to_string_local (events[i], tz, twelve_hour);
        printf ("                       Moonset: %s\n", s);
        free (s);
        DateTime_free (events[i]);
      }
      DateTime_free (start);
      DateTime_free (end);
    }
    printf ("\n");
  }

  if (opt_show_solunar) {
    if (!workingLatlong) {
      fprintf (stderr, "Can't calculate solunar scores because no location has been specified.\n");
      free_day_events (day_events);
      exit (-1);
    }
    if (!datetimeObj) {
      fprintf (stderr, "Can't calculate solunar scores because no date has been specified.\n");
      exit (-1);
    }

    printf ("Solunar\n");

    double phase, age, distance;
    MoonTimes_get_moon_state (datetimeObj, &phase, &age, &distance); 
    double phase_score = Solunar_score_moon_phase (phase);
    double distance_score = Solunar_score_moon_distance (distance);

    printf ("              Moon phase score: %d%%\n", (int)(phase_score * 100.0));
    printf ("           Moon distance score: %d%%", (int)(distance_score * 100.0));
    printf ("\n");

    int dummy, year, month, day;
    DateTime_get_ymdhms (datetimeObj, &year, &month, &day, &dummy, &dummy, &dummy, tz, opt_utc);

    // Work out the maximum and minimum values of solunar and lunar
    //  sine altitude over the whole day 

    DateTime *mn = DateTime_new_dmy_name (day, month, year, "", tz, opt_utc);

    double min_la = 0, max_la = 0, min_sa = 0, max_sa = 0;

    double increment = 0.0;
    for (; increment < 24; increment += 0.5) {
      // We'll take our calculation point as the middle of the
      //  30 minute time period we are displaying

      DateTime *t_center = DateTime_clone (mn);
      DateTime_add_seconds (t_center, 1800 / 2);

      double sa = SunTimes_get_SA (workingLatlong, t_center);
      double la = MoonTimes_get_SA (workingLatlong, t_center);

      if (sa > max_sa) max_sa = sa;
      if (la > max_la) max_la = la;
      if (sa < min_sa) min_sa = sa;
      if (la < min_la) min_la = la;

      DateTime_add_seconds (mn, 1800);
      DateTime_free (t_center);
    }

    DateTime_free (mn);

    //printf ("max la = %f min la = %f max sa = %f min sa = %f\n",
    // max_la, min_la, max_sa, min_sa);

    printf ("\n");
    if (opt_twelvehour) {
      printf ("Time     Sun        Moon       Combined\n");
      printf ("====     ===        ====       ========\n");
    } else {
      printf ("Time  Sun        Moon       Combined\n");
      printf ("====  ===        ====       ========\n");
    }

    mn = DateTime_new_dmy_name (day, month, year, "", tz, opt_utc);
    increment = 0.0;
    double total_combined_score = 0.0;

    BOOL in_solunar_period = FALSE;
    int num_solunar_periods = 0;
    // Since there's only one sun, and it has at most four significant
    //  events (rise, set, under, over), there can't be more than four
    //  active solunar events, even if there are multiple moonrises and
    //  sets in one 24 hour period (which is theoretically possible).

    DateTime *solunar_mid[4];    

    double last_combined_score = 0;
    BOOL got_solunar = FALSE;
    for (; increment < 24; increment += 0.5) {
      char *ts = DateTime_time_to_string_local (mn, tz, twelve_hour);

      // We'll take our calculation point as the middle of the
      //  30 minute time period we are displaying

      DateTime *t_center = DateTime_clone (mn);
      DateTime_add_seconds (t_center, 1800 / 2);

      double sa = SunTimes_get_SA (workingLatlong, t_center);
      double la = MoonTimes_get_SA (workingLatlong, t_center);

      BOOL include_high_noon = TRUE;
      BOOL include_sun_underfoot = FALSE;
      double sunscore = Solunar_score_solar_sa (sa, include_high_noon, include_sun_underfoot, max_sa, min_sa);

      double moonscore = Solunar_score_moon (la, max_la, min_la);
      double combined_score = moonscore * sunscore;
      total_combined_score += combined_score;

      if (in_solunar_period) {
        if (combined_score < last_combined_score) {
          if (!got_solunar) {
            got_solunar = TRUE;
            solunar_mid[num_solunar_periods] = DateTime_clone (t_center);
            DateTime_add_seconds (solunar_mid[num_solunar_periods], -1800);
            num_solunar_periods++;
          }
        }
      }

      if (combined_score > 0.05 && !in_solunar_period) {
        in_solunar_period = TRUE;
      }

      if (combined_score <= 0.05 && in_solunar_period) {
        in_solunar_period = FALSE;
      }

      if (combined_score <= 0.05) {
        got_solunar = FALSE; 
      }

      printf ("%s ", ts);
      printf ("%s ", get_stars (sunscore));
      printf ("%s ", get_stars (moonscore));
      printf ("%s\n", get_stars (combined_score));

      DateTime_add_seconds (mn, 1800);
      DateTime_free (t_center);
      free (ts);
      last_combined_score = combined_score;
    }

    DateTime_free (mn);

   // We get the total coincidence score by integrating the 
   //  combined sun/moon score over the 24-hour period. It's 
   //  very difficult to work out what the maximum value of this
   //  setting. The divisor '5' here is based on a large number of
   //  experiments, such that the maximum value in a year-long period
   //  is 1.00 
    double coincidence_score = total_combined_score / 5; 
    if (coincidence_score > 1.0) coincidence_score = 1.0;

    printf ("\n");
    printf ("     Solunar coincidence score: %d%%\n", (int)(coincidence_score * 100.0));
    printf ("            Solunar peak times:"); 

    if (num_solunar_periods == 0) {
      printf (" none\n");
    } else {
      int i;
      for (i = 0; i < num_solunar_periods; i++) {
        char *s;
        if (opt_syslocal)
          s = DateTime_time_to_string_syslocal (solunar_mid[i], opt_twelvehour);
        else if (opt_utc)
          s = DateTime_time_to_string_UTC (solunar_mid[i], opt_twelvehour);
        else 
          s = DateTime_time_to_string_local (solunar_mid[i], tz, opt_twelvehour);

        printf (" %s", s);
        free (s);
      }
      printf ("\n");
    }


    int i;
    for (i = 0; i < num_solunar_periods; i++) {
      DateTime_free (solunar_mid[i]);    
    }

    printf ("         Overall solunar score: %d%%\n", (int)((coincidence_score + phase_score + distance_score) / 3.0 * 100.0));
  }


  if (datetime) free (datetime);
  if (datetimeObj) DateTime_free (datetimeObj);
  if (latlong) free (latlong);
  if (latlongObj) LatLong_free (latlongObj);
  if (workingLatlong) LatLong_free (workingLatlong);
  if (city) free (city);
  if (cityObj) City_free (cityObj);
  free_day_events (day_events);

  return 0;
}

/*=======================================================================
main
=======================================================================*/
int main (int argc, char **argv)
  {
  static BOOL opt_utc = FALSE;
  static BOOL opt_syslocal = FALSE;
  static BOOL opt_list_named_days = FALSE;
  static BOOL opt_show_solunar = FALSE;
  
  static struct option long_options[] = {
    {"city", required_argument, NULL, 'c'},
    {"datetime", required_argument, NULL, 'd'},
    {"latlong", required_argument, NULL, 'l'},
    {"utc", no_argument, &opt_utc, 'v'},
    {"syslocal", no_argument, &opt_syslocal, 'y'},
    {"days", no_argument, &opt_list_named_days, 0},
    {"solunar", no_argument, &opt_show_solunar, 0},
    {0, 0, 0, 0},
  };

  char *city = NULL;
  char *latlong = NULL;
  char *datetime = NULL;

  while (1) {
    int option_index = 0;
    int opt = getopt_long (argc, argv, "c:l:d:usy", long_options, &option_index);

    if (opt == -1) break;
    switch (opt) {
      case 0: // Long option
        {
          if (strcmp (long_options[option_index].name, "utc") == 0) {
            opt_utc = TRUE;
          }
          if (strcmp (long_options[option_index].name, "syslocal") == 0) {
            opt_syslocal = TRUE;
          }

          if (strcmp (long_options[option_index].name, "days") == 0) {
            opt_list_named_days = TRUE;
          } else if (strcmp (long_options[option_index].name, "solunar") == 0) {
            opt_show_solunar = TRUE;
          } else if (strcmp (long_options[option_index].name, "city") == 0) {
            city = strdup (optarg);
          } else if (strcmp (long_options[option_index].name, "latlong") == 0) {
            latlong = strdup (optarg);
          } else if (strcmp (long_options[option_index].name, "datetime") == 0) {
            datetime = strdup (optarg);
          }
        } // End of long options
        break;
      case 'd':
        datetime = strdup (optarg); 
        break;
      case 'c':
        city = strdup (optarg); 
        break;
      case 'l':
        latlong = strdup (optarg); 
        break;
      case 'u':
        opt_utc = TRUE;
        break;
      case 'y':
        opt_syslocal = TRUE;
        break;
      case 's':
        opt_show_solunar = TRUE;
        break;
      }
    }

    return process(city, latlong, datetime, opt_utc, opt_syslocal, opt_list_named_days, opt_show_solunar);
  }

