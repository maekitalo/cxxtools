////////////////////////////////////////////////////////////////////////
// arg.h
//
// Author: Tommi Maekitalo, Dr. Eckhardt + Partner GmbH
// Datum:  26.3.2002
//

#ifndef ARG_H
#define ARG_H

#if __GNUC__ <= 2
// gcc 2.95 hat kein sstream
#define NO_STRINGSTREAM
#endif

#ifdef NO_STRINGSTREAM
#  include <strstream>
#else
#  include <sstream>
#endif

#include <string>

////////////////////////////////////////////////////////////////////////
/**
 * Auswertung von Kommandozeilenoptionen.
 *
 * Optionen, die auf der Kommandozeile übergeben werden, fangen mit
 * '-' und es folgt ein einzelnes Zeichen und ein Parameter als Wort.
 * Die Klasse entfernt aus der Parameterliste der main-Funktion, die
 * durch die ersten beiden Parameter argc und argv bestimmt wird,
 * Optionen und speichert den angegebenen Wert.
 *
 * Die Optionen können in beliebiger Reihenfolge auf der Kommandozeile
 * angegeben werden.
 *
 * Der Typ des Parameters muß den operator>> (istream&, T&)
 * implementieren.
 *
 * Beispiel:
 * \code
 *   int main(int argc, char* argv[])
 *   {
 *     arg<int> option_n(argc, argv, 'n', 0);
 *     cout << "Wert für -n: " << option_n << endl;
 *   }
 * \endcode
 *
 */
template <class T>
class arg
{
  public:
    /**
     * Auswertung einer Kommandozeilenoption.
     * Der Konstruktor übernimmt von der main-Funktion die ersten beiden
     * Parameter argc und argv und entfernt die ausgewertete Option und
     * deren Parameter. Wird die Option nicht gefunden, wird der default-
     * Wert eingesetzt.
     *
     * \param argc      1. Parameter der main-Funktion
     * \param argv      1. Parameter der main-Funktion
     * \param ch        der Optionsbuchstabe
     * \param def       default-Wert
     *
     * Beispiel:
     * \code
     *  arg<unsigned> offset(argc, argv, 'o', 0);
     *  unsigned wert = ...;
     *  wert += offset;
     * \endcode
     */
    arg(int& argc, char* argv[], char ch, const T& def = T())
    {
      for (int i = 1; i < argc - 1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == ch && argv[i][2] == '\0'
#ifdef NO_STRINGSTREAM
          && !( std::istrstream(argv[i + 1]) >> m_value).fail()
#else
          && !( std::istringstream(argv[i + 1]) >> m_value).fail()
#endif
          )
        {
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }
      
      m_isset = false;
      m_value = def;
    }

    /**
     * Im Gegensatz zum 1. Konstruktor wird hier die Option als String
     * übergeben. Es wird kein Minus-Zeichen angenommen. So können z. B.
     * Langoptionen ausgewertet werden.
     *
     * Beispiel:
     * \code
     *   arg<int> option_nummer(argc, argv, "--nummer", 0);
     *   cout << "nummer =" << option_nummer << endl;
     * \endcode
     */
    arg(int& argc, char* argv[], const char* str, const T& def = T())
    {
      for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], str) == 0
#ifdef NO_STRINGSTREAM
          && !( std::istrstream(argv[i + 1]) >> m_value).fail()
#else
          && !( std::istringstream(argv[i + 1]) >> m_value).fail()
#endif
          )
        {
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }

      m_isset = false;
      m_value = def;
    }

    /**
     * Es werden Minus-Zeichen angenommen:
     *  eine, fuer ein_character_optionen und zwei fuer lange optionen.
     *  So können alternative Optionen ausgewertet werden.
     *
     * Beispiel:
     * \code
     *   arg<int> option_nummer(argc, argv, "n|nummer", 0);
     *   cout << "nummer =" << option_nummer << endl;
     * \endcode
     */
    arg(int& argc, char* argv[], const char* str, char delimiter = '|', const T& def = T())
    {
      for (int i = 1; i < argc - 1; ++i)
      {
        // mindestens ein '-'
        if (argv[i][0] != '-') continue;
        
        const char* ca = &argv[i][1];
        int cLen = strlen(ca);
        if (ca[0] == '-') // nach lange optionen suchen
        {
          ca++;
          cLen--;
          if (1 == cLen) continue;
        }
        else if (1 != cLen) continue;
        
        if (0 == cLen) continue;
        
        const char* p = strstr(str, ca);
        for (;p != NULL; p = strstr(str, ca))
        {
          // Graenzen pruefen
          if ((p == str || delimiter == str[p - str - 1]) &&
              ('\0' == str[p - str + cLen] || delimiter == str[p - str + cLen]))
            break;
          str = p + cLen;
        }
        
        if (p != NULL
#ifdef NO_STRINGSTREAM
          && !( std::istrstream(argv[i + 1]) >> m_value).fail()
#else
          && !( std::istringstream(argv[i + 1]) >> m_value).fail()
#endif
          )
        {
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }
      }

      m_isset = false;
      m_value = def;
    }

    /**
     * Liefert den Wert des Parameters
     */
    const T& getValue() const   { return m_value; }

    /**
     * Liefert den Wert des Parameters
     */
    operator T() const   { return m_value; }

    /**
     * Liefert true zurück, wenn die Option gefunden wurde, also nicht
     * der Default-Wert zum Einsatz kam.
     */
    bool isSet() const   { return m_isset; }

  private:
    T    m_value;
    bool m_isset;
};

////////////////////////////////////////////////////////////////////////
/**
 * Spezialisierung von arg<T> für bool.
 *
 * Optionen auf der Kommandozeile, die lediglich ein- oder ausgeschaltet
 * werden können, werden normalerweise ohne Parameter übergeben. Sie
 * sind einfach entweder da oder nicht.
 *
 * Beispiel:
 * \code
 *
 *  arg<bool> option_t(argc, argv, 't');
 *  if (option_t)
 *    cout << "-t wurde angegeben" << endl;
 *
 *  arg<bool> option_blah(argc, argv, "blah");
 *  if (option_blah)
 *    cout << "blah wurde angegeben" << endl;
 *
 *  arg<bool> option_minus(argc, argv, "-");
 *  if (option_minus)
 *    cout << "- wurde angegeben" << endl;
 *
 * \endcode
 *
 */
class arg<bool>
{
  public:
    /**
     * Konstruktor für die Kurzform. Optionen fangen mit einem Minus-Zeichen
     * an und es folgt genau ein Buchstabe.
     *
     * Mehrere Optionen können zusammengefasst werden. Die Optionengruppe
     * muß mit einem '-' anfangen, aber nicht mit '--' und den angegebenen
     * Optionsbuchstaben enthalten
     */
    arg(int& argc, char* argv[], char ch)
    {
      for (int i = 1; i < argc; ++i)
        if (argv[i][0] == '-' && argv[i][1] != '-')
        {
          // fängt mit einem '-' an, aber nicht mit '--'
          if (argv[i][1] == ch && argv[i][2] == '\0')
          {
            // das ist eine allein stehende Option
            m_value = true;
            m_isset = true;
            for ( ; i < argc - 1; ++i)
              argv[i] = argv[i + 1];
            argc -= 1;
            argv[argc] = 0;
            return;
          }
          else if (argv[i][1] == ch && argv[i][2] == '-' && argv[i][3] == '\0')
          {
            // Option wurde explizit mit -x- ausgeschaltet
            m_value = false;
            m_isset = true;
            for ( ; i < argc - 1; ++i)
              argv[i] = argv[i + 1];
            argc -= 1;
            argv[argc] = 0;
            return;
          }
          else
          {
            // allein steht das nicht, aber mal sehen, ob der Optionsbuchstabe
            // in der Gruppe vorkommt
            for (char* p = argv[i] + 1; *p != '\0'; ++p)
              if (*p == ch)
              {
                // na da haben wir ihn; weg damit und fertig
                m_value = true;
                m_isset = true;
                do
                {
                  *p = *(p + 1);
                } while (*p++ != '\0');

                return;
              }
          }
        }
      m_value = false;
      m_isset = false;
    }

    /**
     * Konstruktor für die Langform. Die Option besteht aus einem beliebigen
     * Text.
     */
    arg(int& argc, char* argv[], const char* str)
    {
      for (int i = 1; i < argc; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          m_value = true;
          m_isset = true;
          for ( ; i < argc - 1; ++i)
            argv[i] = argv[i + 1];
          argc -= 1;
          argv[argc] = 0;
          return;
        }
      m_value = false;
      m_isset = false;
    }

    /**
     * Liefert true, wenn die Option gesetzt ist.
     */
    bool isTrue() const   { return m_value; }

    /**
     * Liefert true, wenn die Option nicht gesetzt ist.
     */
    bool isFalse() const  { return !m_value; }

    /**
     * Liefert true, wenn die Option gesetzt ist.
     */
    operator bool() const  { return m_value; }

    /**
     * Liefert true zurück, wenn die Option explizit gesetzt.
     */
    bool isSet() const             { return m_isset; }

  private:
    bool m_value;
    bool m_isset;
};

////////////////////////////////////////////////////////////////////////
/**
 * Spezialisierung von arg<T> für const char*
 *
 * "const char*" werden nicht über einen istream gelesen, sondern direkt
 * verarbeitet.
 */
class arg<const char*>
{
  public:
    /**
     * Konstruktor für die Kurzform. Optionen fangen mit einem
     * Minus-Zeichen an und es folgt ein einzelner Buchstabe und
     * ein Wert.
     */
    arg(int& argc, char* argv[], char ch, const char* def = 0)
    {
      for (int i = 1; i < argc - 1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == ch && argv[i][2] == '\0')
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }

      m_isset = false;
      m_value = def;
    }

    /**
     * Konstruktor für die Langform. Die Option besteht aus beliebigen
     * Text. Die Option muß nicht mit einem Minus-Zeichen anfangen.
     * Es folgt ein Parameterwert.
     */
    arg(int& argc, char* argv[], const char* str, const char* def)
    {
      for (int i = 1; i < argc - 1; ++i)
        if (strcmp(argv[i], str) == 0)
        {
          m_value = argv[i + 1];
          m_isset = true;
          for ( ; i < argc - 2; ++i)
            argv[i] = argv[i + 2];
          argc -= 2;
          argv[argc] = 0;
          return;
        }

      m_isset = false;
      m_value = def;
    }

    /**
     * Liefert den Wert des Parameters
     */
    const char* getValue() const   { return m_value; }

    /**
     * Liefert den Parameterwert.
     */
    operator const char*() const   { return m_value; }

    /**
     * Liefert true zurück, wenn die Option gefunden wurde, also nicht
     * der Default-Wert zum Einsatz kam.
     */
    bool isSet() const             { return m_isset; }

  private:
    const char* m_value;
    bool        m_isset;
};

#endif // ARG_H
