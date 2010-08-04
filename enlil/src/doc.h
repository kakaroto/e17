/**
 * @mainpage EPhoto Manager
 * @author Jonathan Atton
 * @date 2009
 *
 * The Documentation is not yet available. Here Some information and advises are
 * list.
 *
 * This library is written for my particulars, that's means I write the methods I need. If you
 * need something more contact me or add it.
 *
 * The library is a photo manager. It can create a database of albums and photos.
 * The database is a list of eet files :
 * <ul>
 * <li> 1 in the root directory with the list of albums.
 * <li> 1 by album in the albums directory with the complete album description
 * and the list of photos.
 * </ul>
 *
 * The organisation in the hard drive is :
 * @code
 * root directory
 *         - album 1
 *                   - photo 1
 *                   - photo 2
 *                   - data.eet
 *         - album 2
 *                   - photo 1
 *                   - data.eet
 *         - data.eet
 * @endcode
 *
 *
 * @section eet_file The Eet files
 *
 * Never use eet_open() and eet_close(). Instead use pm_file_manager_open()
 * and pm_file_manager_close(). These methods use a mutex to avoid a conflict
 * between the threads and it use a LRU list to increase the speed (the files are not
 * immediately closed).
 *
 * @section loader The loader : Photo_Manager_Load
 *
 * The loader load from the Eet files the albums and the photos of a root
 * directory. The loader run in a thread but is not thread safe, consequently you should never modified
 * the data before the loading is done.
 *
 * @section sync The synchronizer
 *
 * The synchroniser is running in a thread and is thread safe.The thread use
 * its own data. It compare the content of a Eet file and the content of a
 * folder. When a change is detected (new album, deleted photo ...) the thread
 * send a message to the main thread and stop during the time the message is
 * processed.
 *
 * For example if a new photo is detected the thread send you the new photo
 * and you can copy it before the thread continue his job.
 *
 * The synchroniser can be launch on a root folder, an album or a photo.
 *
 * @section gb Good practices
 *
 * You should never create a new photo or album (pm_album_new()). For example
 * if you need to add a new photo in an album, just create the file in the
 * hard drive. The monitor of the album will detect the file, then you launch
 * the synchroniser on it and the Eet file will be automatically update.
 *
 *
 */

