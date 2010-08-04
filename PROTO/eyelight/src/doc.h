#ifndef  DOC_INC
#define  DOC_INC

/**
 * @mainpage Eyelight
 * @author Jonathan Atton
 * @date 2008-2009
 *
 * @section eyelight_toc_sec Table of contents
 *
 * <ul>
 *  <li> @ref eyelight_smart_object_sec
 *  <li> @ref eyelight_sec
 *  <li> @ref eyelight_edit_sec
 * </ul>
 *
 *
 * @section eyelight_intro_sec Introduction
 *
 * Eyelight is a presentation viewer using Edje to render the slides. The slides are created in a text file (an ELT file), which use a syntax closed to the syntax used in a EDC file, then the viewer parse and compile this file and display the presentation.
 *
 * Eyelight was started during the Google Summer of Code 2008. The initial aims was to create an application which read a text file, parse it and then display an Edje presentation. Eyelight has been modified a lot since this first version :
 * <ul>
 * <li> At the beginning, Eyelight created an edc file from the presentation file, the result was a list of macros call. The theme file was a list of macros which was called by the edc presentation file. And finally we used edje_cc to create an edj file and we get the presentation. Now the theme file is a classic edj file with a list of groups (text item, image, video ...) and the viewer load theses group when necessary (item text -> load the group text item) like all edje applications do.
 * <li> A Eyelight smart object has been created. This means you can use Eyelight in all EFL applications (Elementary, Edje ...).
 * <li> The API Eyelight_Edit appears. This API allows to edit a presentation on the fly : add a slide, delete an area ... Then the new presentation can be saved into an ELT file.
 * </ul>
 *
 * @image html "diagram.png" "Architecture of Eyelight"
 *
 * @section eyelight_smart_object_sec Eyelight smart object
 *
 * The Eyelight smart object API is in the header : Eyelight_Smart.h.
 *
 * It is a classic evas smart object, which means it can be used in an Evas/Edje/Elementary application. Each object are independant consequently you can display more than 1 presentation in an application. Do not forget to initialise Eyelight with eyelight_init().
 *
 * @section eyelight_sec Eyelight
 *
 * The Eyelight API is in the header : Eyelight.h
 *
 * It is the main API of Eyelight. We can use this API to display a presentation but it is recommended to use the smart object because it is more easy.
 *
 * @section eyelight_edit_sec Eyelight edit
 *
 * The Eyelight edit API is in the header : Eyelight_Edit.h
 *
 * This API allows to modify the presentation on the fly : add a new slide, delete an area, add an image ...  This API is used by Eyelight_editor and is not complete.
 *
 * <b>Warning : this API only works on the slide displayed by the presentation. Do not use it on an other slide ! </b>
 *
 *
 * @section eyelight_screenshots Some screenshots
 *
 *  @image html "screens.png"
 *
 * @section eyelight_contact Contact
 *
 * Jonathan Atton alias Watchwolf <jonathan[dot]atton[at]gmail[dot]com>
 *
 */

/**
 * @page tutorial_presentation How to write a presentation
 *
 * @section eyelight_pres_table Table of contents
 *
 * <ul>
 * <li> @ref general_presentation
 * <li> @ref tutorial_presentation_slides
 * <ul>
 *      <li> @ref tutorial_presentation_area
 *      <ul>
 *          <li> @ref tutorial_presentation_text
 *          <li> @ref tutorial_presentation_items
 *          <ul>
 *              <li> @ref tutorial_presentation_item
 *          </ul>
 *          <li> @ref tutorial_presentation_image
 *          <li> @ref tutorial_presentation_edje
 *          <li> @ref tutorial_presentation_video
 *          <li> @ref tutorial_presentation_presentation
 *      </ul>
 * </ul>
 * <li> @ref tutorial_presentation_default_values
 * </ul>
 *
 * @section general_presentation General presentation
 *
 * A presentation file has the extension .elt and is an ASCII file. The file contains a list of blocks, properties and values.
 *
 * An example:
 *
 * @image html "screen_tuto_pres.png"
 *
 * @code
 * slide {
 *      layout: "2_columns_left";
 *      title: "Creating a presentation";
 *      subtitle: "a beautiful presentation !";
 *      transition_next: fade;
 *      area {
 *          name:left;
 *          layout: vertical;
 *          items {
 *              numbering: roman;
 *              item {
 *                  text: "The presentation is an <b>elt file</b>";
 *              }
 *              item {
 *                  text: "Theme and content are independent";
 *              }
 *              text: "Use some blocks and properties to construct your slide: ";
 *              items {
 *                  text: "blocks slide, items, image ...";
 *                  text: "properties title, image, text ...";
 *              }
 *          }
 *      }
 *      area {
 *          name: right;
 *          image: "code_presentation.png";
 *      }
 * }
 * @endcode
 *
 *
 *
 *
 *
 * @section tutorial_presentation_slides A list of slides
 *
 * A presentation is a list of block slides:
 *
 * @code
 * slide {
 *      title: "slide 1";
 * }
 * slide {
 *      title: "slide 2";
 * }
 * @endcode
 *
 * The block slide has a list of properties and blocks:
 *
 * <table border=0><tr><td><b>Property</b></td><td><b>Description</b></td></tr>
 * <tr><td> title: "a title"; </td><td> define the title of the slide </td></tr>
 * <tr><td> subtitle: "a subtitle"; </td><td> define the subtitle of the slide </td></tr>
 * <tr><td> header_image: "an image"; </td><td> define the image in the header of the slide </td></tr>
 * <tr><td> foot_text: "a text"; </td><td> define the text in the foot </td></tr>
 * <tr><td> foot_image: "an image"; </td><td> define the image in the foot </td></tr>
 * <tr><td> layout: "2_columns"; </td><td> see the page @ref tutorial_layout for more information </td></tr>
 * <tr><td> custom_area: "center" 0.4 0.4 0.6 0.6; </td><td> Add a custom area in the slide. </td></tr>
 * <tr><td> transition: "vertical"; </td><td> define the previous and next transition </td></tr>
 * <tr><td> transition_next: "horizontal"; </td><td> define the next transition </td></tr>
 * <tr><td> transition_previous: "fade"; </td><td> define the previous transition </td></tr>
 * <tr><td><b>Block</b></td><td><b>Description</b></td></tr>
 * <tr><td> area </td><td> An area is a zone defined in a slide. See @ref tutorial_presentation_area.</td></tr>
 * </table>
 *
 * @code
 * slide {
 *      layout: 2_columns_left;
 *      foot_text :"Watchwolf , watchwolf@watchwolf.fr";
 *      foot_image: "wolf.png";
 *      header_image: "logo.png";
 *      title: "<c>Eyelight: a presentation tools</c>";
 *      subtitle: "<c>When edje meets a presentation</c>";
 * }
 * @endcode
 *
 *
 *
 * @section tutorial_presentation_area The block area
 *
 * An area is a zone in a slide where we can add a list of items (text, image ...). Each area has a layout : vertical, horizontal ... (we can use all the layouts defined by default in Evas). A slide layout defines a list of default areas. For example the slide layout "2_columns" defines 2 areas : left and right.
 *
 * The list of properties and blocks :
 * <table border=0><tr><td><b>Property</b></td><td><b>Description</b></td></tr>
 * <tr><td>name: "left"; </td><td> The name of the area </td></tr>
 * <tr><td> layout: "vertical/horizontal"; </td><td> defined how we want add the objects in the slide, default value: vertical </td></tr>
 * <tr><td><b>Block</b></td><td><b>Description</b></td></tr>
 * <tr><td> text </td><td> see @ref tutorial_presentation_text </td></tr>
 * <tr><td> items </td><td> see @ref tutorial_presentation_items </td></tr>
 * <tr><td> image </td><td> see @ref tutorial_presentation_image </td></tr>
 * <tr><td> edje </td><td> see @ref tutorial_presentation_edje </td></tr>
 * <tr><td> video </td><td> see @ref tutorial_presentation_video </td></tr>
 * <tr><td> presentation </td><td> see @ref tutorial_presentation_presentation </td></tr>
 * </table>
 *
 *
 *
 * @section tutorial_presentation_text A simple text item
 *
 * A simple text item can be added in an area.
 *
 * @code
 * text: "A text :)";
 * @endcode
 *
 * @code
 * slide {
 *      layout: blank;
 *      title: "3 customs areas";
 *      custom_area: "center" 0.4 0.4 0.6 0.6;
 *      area {
 *          name:"center";
 *          text:"hehe";
 *      }
 * }
 * @endcode
 *
 *
 * @section tutorial_presentation_items The block items
 *
 * A list of items can be added in an area or in a list of items. The list can be numbering or not.
 *
 * @code
 * items {
 *      numbering: "none";
 *      item: "first item";
 *      items {
 *          numbering: "normal";
 *          item: "first sub item";
 *          item: "second sub item";
 *      }
 *      item: "last item";
 * }
 * @endcode
 *
 * The list of properties and blocks :
 * <table border=0><tr><td><b>Property</b></td><td><b>Description</b></td></tr>
 * <tr><td> numbering: "none|normal|roman" </td><td> specify if the items are numbering and the style. The default value is none.</td></tr>
 * <tr><td><b>Block</b></td><td><b>Description</b></td></tr>
 * <tr><td> item </td><td> See @ref tutorial_presentation_item </td></tr>
 * </table>
 *
 * @section tutorial_presentation_item An item
 *
 * A block item defines a simple text in a list of items. The shortcut "item: text" can be used to avoid defining a complete block. Currently the block item has no properties, consequently this is useless to use the block, the shortcut is enough.
 *
 * @code
 * items {
 *      item: "first item";
 *      item {
 *          text: "second item";
 *      }
 *      item: "last item";
 * }
 * @endcode
 *
 *
 *
 * @section tutorial_presentation_image An image
 *
 * An image can be added in an area by using a block or a shortcut. With the block a shadow and/or a border can be display.
 *
 * @code
 * area {
 *      image: "image_1.png";
 *      image {
 *          image: "images/image_2.png";
 *          shadow: 1;
 *          border: 1;
 *          aspect: 3 1;
 *          keep_aspect: 1;
 *      }
 * }
 * @endcode
 *
 * The block image has 3 properties:
 * <table border=0><tr><td><b>Property</b></td><td><b>Description</b></td></tr>
 * <tr><td> image: "file.jpg"; </td><td> The image file. </td></tr>
 * <tr><td> shadow: 1|0 </td><td> 1 to display a shadow. The default value is 0. </td></tr>
 * <tr><td> border: 1|0 </td><td> 1 to display a border. The default value is 0. </td></tr>
 * <tr><td> aspect: float:x float:y) </td><td> Same as Edje, When the with of the image is increase of x pixels, the height is increase of y pixels. Defaults values are 0 0. If this attribute is specified, the attribute keep_aspect is deactivated. </td></tr>
 * <tr><td> keep_aspect: 1|0 </td><td> If 1, calculate the correct aspects values to keep the original aspect of the image. The default value is 1. </td></tr>
 * </table>
 *
 *
 * @section tutorial_presentation_edje An Edje file
 *
 * An edje item can be added in an area by using a block. The item is defined by a *.edj file and a group.
 *
 * @code
 * area {
 *      edje {
 *          file: "edje/file.edj";
 *          group: "my_edje/icon";
 *      }
 * }
 * @endcode
 *
 * The block edje has 2 properties:
 * <table border=0><tr><td><b>Property</b></td><td><b>Description</b></td></tr>
 * <tr><td> file: "file.edj"; </td><td> The edje file. </td></tr>
 * <tr><td> group: "a group" </td><td> The group to display. </td></tr>
 * </table>
 *
 *
 * @section tutorial_presentation_video A Video
 *
 * A video can be added in an area by using a block. The item is defined by the video file and some properties can be defined.
 *
 * @code
 * area {
 *      video {
 *          file: "video/last_week.avi";
 *          alpha: 255;
 *          autoplay: 1;
 *          replay: 1;
 *          shadow: 1;
 *          border: 1;
 *      }
 * }
 * @endcode
 *
 * The block edje has 6 properties:
 * <table border=0><tr><td><b>Property</b></td><td><b>Description</b></td></tr>
 * <tr><td> file: "file.avi"; </td><td> The video file. </td></tr>
 * <tr><td> alpha: 0-255 </td><td> The alpha applied to the video. </td></tr>
 * <tr><td> autoplay: 0|1 </td><td> If the video is automatically played when the slide is display. The default value is 0.</td></tr>
 * <tr><td> replay: 0|1 </td><td> If the video is automatically replayed when the video is at the end. The default value is 0.</td></tr>
 * <tr><td> shadow: 0|1 </td><td> 1 to display a shadow. The default value is 0. </td></tr>
 * <tr><td> border: 0|1 </td><td> 1 to display a border. The default value is 0. </td></tr>
 * </table>
 *
 *
 *
 * @section tutorial_presentation_presentation An Eyelight presentation
 *
 * A Eyelight presentation can be added in an area by using a block. The item is defined by the presentation file (an elt file).
 *
 * @code
 * area {
 *      presentation {
 *          file: "pres/eyelight.elt";
 *          theme: "an eyelight theme.edj"
 *          shadow: 1;
 *          border: 1;
 *      }
 * }
 * @endcode
 *
 * The block presentation has 4 properties:
 * <table border=0><tr><td><b>Property</b></td><td><b>Description</b></td></tr>
 * <tr><td> file: "file.elt"; </td><td> The presentation file. </td></tr>
 * <tr><td> theme: "theme.edj" </td><td> An Eyelight theme. The default eyelight theme is used as default value.</td></tr>
 * <tr><td> shadow: 0|1 </td><td> 1 to display a shadow. The default value is 0. </td></tr>
 * <tr><td> border: 0|1 </td><td> 1 to display a border. The default value is 0. </td></tr>
 * </table>
 *
 *
 *
 * @section tutorial_presentation_default_values The default values
 *
 * In the file (not inside a block), we can define a list of default values for the properties: layout (of the slide), foot_text, foot_image, header_image, title, subtitle, transition, transition_next, transition_previous.
 *
 * These values are used for the next slides, for example if we define a default value after the 5th slide, this value will be use only after the 5th slide.
 *
 * @code
 * //a list of defaults value
 * layout: 2_columns_left;
 * foot_text :"Watchwolf , watchwolf@watchwolf.fr";
 * foot_image: "wolf.png";
 * header_image: "logo.png";
 * title: "<c>Eyelight: a presentation tools</c>";
 * subtitle: "<c>When edje meets a presentation</c>";
 * transition: fade;
 * transition_next: horizontal;
 * @endcode
 *
 */


/**
 * @page tutorial_layout Layouts of Eyelight
 * @section tutorial_slide_layout Layouts of a slide
 * @section tutorial_area_layout Layouts of an area
 */

#endif   /* ----- #ifndef DOC_INC  ----- */

