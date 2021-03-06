#ifndef ArduCor_h
#define ArduCor_h

#include "Arduino.h"
#include "ArduCorProtocols.h"

/*!
 * \version v3.0.0
 * \date April 14, 2018
 * \author Tim Seemann
 * \copyright <a href="https://github.com/timsee/ArduCor/blob/master/LICENSE">
 *            MIT License
 *            </a>
 *
 *
 * \brief An Arduino library that provides a set of RGB lighting routines for compatible LED
 *        array hardware.
 *
 * \details This library has been tested with SeeedStudio Rainbowduinos, quite a few of the
 * Adafruit Neopixels products, and a standard RGB LED. Sample code is provided in the git
 * repo for all tested hardware in the samples folder of the git repository.
 *
 * If you are starting a project from scratch, first you'll need to make a global object in the
 * arduino sketch:
 *
 * ~~~~~~~~~~~~~~~~~~~~~
 * ArduCor routines = ArduCor(LED_COUNT);
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * where `LED_COUNT` is the number of LEDs in your array.
 *
 * The library produces lighting routines based on the functions used and stores the routine
 * in its internal buffers. These buffers can then be accessed by getters and displayed
 * on the LED hardware. For routines that change over time, this process should be repeated
 * on a loop. For example, here is how you would make a red blinking light with the library
 * and a Neopixels board:
 *
 * First, call this function to store the routine in the library's internal buffers:
 *
 * ~~~~~~~~~~~~~~~~~~~~~
 * routines.singleBlink(255, 0, 0);
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * Then, update the LED array with the values from the library's RGB buffer. The way to do this will
 * vary from hardware to hardware, but for a NeoPixels sample, it would look something like this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~
 * routines.applyBrightness();  // Optional. Dims the LEDs based on the routines.brightness() setting
 * for (int x = 0; x < LED_COUNT; x++) {
 *    pixels.setPixelColor(x, pixels.Color(routines.red(x),
 *                                         routines.green(x),
 *                                         routines.blue(x)));
 * }
 * pixels.show();
 * ~~~~~~~~~~~~~~~~~~~~~
 *
 * By this point, the LEDs should be showing red. To achieve the blink effect, put both of
 * these in your `loop()` function and then put a delay between updates. This delay will
 * be used to determine how fast the LED's blink.
 *
 *
 */
class ArduCor
{
public:

    //================================================================================
    // Structs and Constructors
    //================================================================================

    // used to store a color value
    struct Color
    {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    /*!
     * Required constructor. The library should be stored in
     * global memory and allocated only once at startup.
     *
     * It will allocate `4 * ledCount` bytes.
     *
     * \param ledCount number of individual RGB LEDs.
     */
    ArduCor(uint16_t ledCount);

    /*!
     * Resets all internal values to the original values.
     */
    void resetToDefaults();

    /*!
     *  Turns on all the LEDs.
     */
    void turnOn();

    /*!
     *  Turns off all the LEDs. To turn the lights back on, call any
     *  light routine or call `turnOn()`.
     */
    void turnOff();

    //================================================================================
    // Getters and Setters
    //================================================================================
    /*! @defgroup gettersetters Getters and Setters
     *  These are the getters and setters for ArduCor that are used to control
     *  the settings and the colors.
     *  @{
     */

    /*!
     * Sets the color used for single color routines. This is automatically
     * called by each routine. Returns false if the new main color matches
     * the previous main color.
     *
     * \return true if a new color is set, false if the input matches the current color.
     */
    bool setMainColor(uint8_t r, uint8_t g, uint8_t b);

    /*!
     * Set the color in the custom color array at the provided index. colorIndex must be
     * less than the size of the custom color array or else it won't have any effect.
     */
    void setColor(uint16_t colorIndex, uint8_t r, uint8_t g, uint8_t b);

    /*!
     * Sets the amount of colors used in custom multi color routines. The value given must
     * be less than the size of the custom array or else it will be set to use the entire
     * array.
     */
    void setCustomColorCount(uint8_t count);

    /*!
     * Returns true if the LEDs are on, false if they are off.
     */
    boolean isOn() { return m_is_on; }

    /*!
     * Retrieve the amount of colors that are used from the custom array.
     */
    uint8_t customColorCount();

    /*!
     * Set the  palette brightness between 0 and 100. 0 is off, 100 is full brightness. Note
     * this only impacts multi color routines.
     */
    void brightness(uint8_t brightness);

    /*!
     * Retrieve the palette brightness level, which is a value between 0 and 100 where
     * 100 is full brightness.
     */
    int brightness() { return m_bright_level; }

    /*!
     * Retrieve the main color, which is used for single color routines.
     */
    Color mainColor();

    /*!
     * Retrieve the color at the given index.
     */
    Color color(uint16_t i);

    /*!
     * Retrieve the r value at a given index in the buffer.
     */
    uint8_t red(uint16_t i);

    /*!
     * Retrieve the g value at a given index in the buffer.
     */
    uint8_t green(uint16_t i);

    /*!
     * Retrieve the b value at a given index in the buffer.
     */
    uint8_t blue(uint16_t i);

    /*! @} */
    //================================================================================
    // Single Color Routines
    //================================================================================
    /*! @defgroup singleRoutines Single Color Routines
     *  These routines each take a R, G, and B value as parameters to generate
     *  a color. This color is the only color used by the routine.
     *
     *  All routines except singleSolid should be called repeatedly on a loop for their
     *  full effect. The speed of the loop determines how fast the LEDs update.
     *  @{
     */

    /*!
     * Set every LED to the provided color.
     *
     * \param red strength of red LED, between 0 and 255
     * \param green strength of green LED, between 0 and 255
     * \param blue strength of blue LED, between 0 and 255
     */
    void singleSolid(uint8_t red, uint8_t green, uint8_t blue);

    /*!
     * Switches between ON and OFF states using the provided color.
     *
     * \param red strength of red LED, between 0 and 255
     * \param green strength of green LED, between 0 and 255
     * \param blue strength of blue LED, between 0 and 255
     */
    void singleBlink(uint8_t red, uint8_t green, uint8_t blue);

    /*!
     * Uses the provided color and generates groups of the color in increasing levels
     * of brightness. On each update, the LEDs move one index to the right. This creates a
     * wave/scrolling effect.
     *
     * \param red strength of red LED, between 0 and 255
     * \param green strength of green LED, between 0 and 255
     * \param blue strength of blue LED, between 0 and 255
     */
    void singleWave(uint8_t red, uint8_t green, uint8_t blue);

    /*!
     * Set every LED to the provided color. A subset of the LEDs based on the
     * percent parameter will be less bright than the rest of the LEDs.
     *
     * \param red strength of red LED, between 0 and 255
     * \param green strength of green LED, between 0 and 255
     * \param blue strength of blue LED, between 0 and 255
     * \param percent determines how many LEDs will be slightly dimmer than the rest, between 0 and 100
     */
    void singleGlimmer(uint8_t red, uint8_t green, uint8_t blue, uint8_t percent = 20);


    /*!
     * Fades the LEDs in and out based on the provided color.
     * Can fade in two ways: linear and sine. If `isSine` is set to false, the interval
     * between each update is constant. If `isSine` is true, a sine wave is used to generate
     * the intervals, resulting in lights that stay on near their full brightness for longer.
     *
     * \param red strength of red LED, between 0 and 255
     * \param green strength of green LED, between 0 and 255
     * \param blue strength of blue LED, between 0 and 255
     * \param isSine if true, a sine wave is used, if false, constant intervals are used.
     */
    void singleFade(uint8_t red, uint8_t green, uint8_t blue, bool isSine);

    /*!
     * If `fadeIn` is true, the LEDs start with a brightness value of 0 and each update rasies
     * the brightness by a constant value.  When it reaches maximum brightness, it resets the
     * brightness back to 0 and repeats the fade in.  If `fadeIn` is set to false, it does the
     * opposite; it starts at full brightness and then fades to darkness.
     *
     * \param red strength of red LED, between 0 and 255
     * \param green strength of green LED, between 0 and 255
     * \param blue strength of blue LED, between 0 and 255
     * \param fadeIn if true, it fades from darkness to maximum brightness, if false,
     *        it fades from maximum brightness to darkness.
     */
    void singleSawtoothFade(uint8_t red, uint8_t green, uint8_t blue, bool fadeIn);



    /*! @} */
    //================================================================================
    // Multi Color Routines
    //================================================================================
    /*! @defgroup multiRoutines Multi Colors Routines
     *
     *  These routines use multiple colors.
     *  They all take the parameter of `palette` which is used to determine which set of
     *  colors to use. The custom color array is eCustom, all other values for `palette`
     *  come from groups of preset colors. Go to the project's github for a full list
     *  of the palettes and their corresponding values.
     *
     *  All routines except multiBarsSolid should be called repeatedly on a loop
     *  for their full effect. The speed of the loop determines how fast the LEDs update.
     *  @{
     */

    /*!
     * This method uses its percent parameter to dim LEDs randomly, similar to the
     * standard glimmer mode. It also uses the percent to randomly change the color
     * of select LEDs to a color in the chosen palette. The base color is the first
     * from the chosen palette.
     *
     * \param palette the palette to use for the routine. eCustom is the custom array,
     *        all other values are preset groups.
     * \param percent percent of LEDs that will get the glimmer applied, between 0 and 100
     */
    void multiGlimmer(EPalette palette, uint8_t percent = 20);

    /*!
     * Fades between all the colors used by the palette.
     *
     * \param palette the palette to use for the routine. eCustom is the custom array,
     *        all other values are preset groups.
     */
    void multiFade(EPalette palette);

    /*!
     * sets each individual LED as a random color from the chosen palette.
     *
     * \param palette the color array to use for the routine. eCustom is the custom array,
     *        all other values are palette arrays.
     */
    void multiRandomIndividual(EPalette palette);

    /*!
     * A random color is chosen from the chosen palette and applied to each LED.
     *
     * \param palette the palette to use for the routine. eCustom is the custom array,
     *        all other values are preset groups.
     */
    void multiRandomSolid(EPalette palette);

    /*!
     * Uses the chosen palette to set the LEDs in alternating patches with a size of barSize.
     * On each update, the bars move up one LED index on each frame update to
     * create a "scrolling" effect.
     *
     * \param palette the palette to use for the routine. eCustom is the custom array,
     *        all other values are preset groups.
     * \param barSize how many LEDs before switching to the other bar.
     */
    void multiBars(EPalette palette, uint8_t barSizeSetting);

    /*! @} */
    //================================================================================
    // Post Processing
    //================================================================================
    /*! @defgroup postProcessing Post Processing
     *
     *  These methods can be called after a routine is chosen but before the routines
     *  get displayed to the LEDs. They add special effects to the routines.
     */

    /*!
     * This function takes the brightness() value given to the routines object and applies
     * it to every LED. Relatively speaking, this is a pretty expensive operation so it is
     * left optional.
     */
    void applyBrightness();

    /*!
     * Attempts to draw the color provided on the index provided.
     *
     * \param i the index of the LED that you want to change. Must be less than the total
     *        amount of LEDs or else it will return false.
     * \param red the new red value of the LED, between 0 and 255.
     * \param green the new green value of the LED, between 0 and 255.
     * \param blue the new blue value of the LED, between 0 and 255.
     * \return true if index exists and the color was drawn, false otherwise.
     */
    bool drawColor(uint16_t i, uint8_t red, uint8_t green, uint8_t blue);

    /*! @} */
private:

    // used by multi color routines to store their colors.
    Color    m_temp_array[10];
    // stores the user's settings for custom colors.
    Color    m_custom_colors[10];
    // stores how many colors from the custom colors array should
    // be used in a routine.
    uint8_t  m_custom_count;

    // these variables are checked in every preproces step
    ERoutine  m_current_routine;
    EPalette  m_current_palette;

    // used for single color routines
    Color m_main_color;

    // buffers used for storing the RGB LED values
    uint8_t *r_buffer;
    uint8_t *g_buffer;
    uint8_t *b_buffer;

    // settings and stored values
    uint16_t m_LED_count;
    uint16_t m_bar_size;
    uint16_t m_bright_level;
    uint8_t  m_fade_speed;
    uint8_t  m_blink_speed;
    boolean  m_brightness_flag;
    boolean  m_preprocess_flag;
    boolean  m_is_on;

    // temp values
    uint8_t *m_temp_buffer;
    uint16_t m_temp_counter;
    uint16_t m_temp_index;
    boolean  m_temp_bool;
    Color    m_temp_color;
    uint8_t  m_temp_size;
    uint8_t  m_temp_goal;
    int      m_temp_step;
    float    m_temp_float;

    // variables used by specific routines
    Color    m_goal_color;
    int      m_red_diff;
    int      m_green_diff;
    int      m_blue_diff;
    uint8_t  m_fade_counter;
    uint16_t m_loop_index;
    uint8_t  m_loop_count;
    uint8_t  m_scale_factor;
    uint8_t  m_repeat_index;

    uint8_t  m_possible_array_color;

    // index for loops and other iterators
    uint16_t x;

    /*!
     * Called before every function. Used to update the library state tracking
     * and to reset any necessary variables when a state changes.
     *
     * \param routine the routine that is about to be displayed
     * \param the palette that will be used. For single color routines,
     *        this value is ignored.
     */
    void preProcess(ERoutine routine, EPalette palette);

    /*!
     * Called by preprocessing if the the palette has changed. This sets up the
     * m_temp_array and m_temp_size with the relevant data from the color palette.
     *
     * \palette the new palette that is getting used by the routines.
     */
    void setupPalette(EPalette palette);

    /*!
     * Sets two colors alternating in patches the size of barSize.
     * and moves them up in index on each frame.
     *
     * \param colorCount the number of colors in the array used for the routine.
     * \param groupSize how many LEDs before switching to the other bar.
     * \param startingValue the lowest possible value used by the moving buffer. Must
     *        be less than colorCount or otherwise it defaults to zero.
     */
    void movingBufferSetup(uint16_t colorCount, byte groupSize, uint8_t startingValue = 0);


    /*!
     * Chooses a random different color from the array of colors. Stores resulting color in
     * m_temp_color and the random index in m_temp_index.
     *
     * \param array pointer to the color array.
     * \param the largest possible value that can be randomly chosen, must be less than
     *        the size of the color array.
     * \param true if it can have the same value as the last time this was called, false otherwise.
     *        For example, if this is set to true and the last time a value was chosen it chose
     *        2, then the next value will be anything else in the range except 2.
     */
    void chooseRandomFromArray(Color *array, uint8_t max_index, boolean canRepeat);

    /*!
     * Uses memset to change every value in the r_buffer to r, the g_buffer to g, and the b_buffer
     * to b. All previous colors in the buffers will be overwritten by this function call.
     *
     * \param r the new value for all red LEDs.
     * \param g the new value for all green LEDs.
     * \param b the new value for all blue LEDs.
     */
    void fillColorBuffers(uint8_t r, uint8_t g, uint8_t b);

    /*!
     * Sets the size of bars in routines that use them. Bars are groups of LEDs that
     * all display the same color. The routines SingleWave, MultiBarsSolid, and
     * MultiBarsMoving use them.
     *
     * \barSize a number greater than 0 and less than the number of LEDs being used.
     */
    void barSize(uint8_t barSize);
};

#endif //ArduCor_h
