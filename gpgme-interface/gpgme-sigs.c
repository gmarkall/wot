/* Example listing signatures
 *
 * Originally based on the gpgme-example1.c from
 *
 * http://www.nico.schottelius.org/docs/a-small-introduction-for-using-gpgme/
 *
 * by Nico Schottelius
 *
 * 2007-08-05 GPLv3
 *
 */

#include <gpgme.h>
#include <stdio.h>
#include <locale.h>

int setup(gpgme_ctx_t* ctx)
{
   char *p;
   gpgme_error_t err;
   gpgme_engine_info_t enginfo;

   setlocale (LC_ALL, "");
   p = (char *) gpgme_check_version(NULL);
   printf("version=%s\n",p);
   /* set locale, because tests do also */
   gpgme_set_locale(NULL, LC_CTYPE, setlocale (LC_CTYPE, NULL));

   /* check for OpenPGP support */
   err = gpgme_engine_check_version(GPGME_PROTOCOL_OpenPGP);
   if(err != GPG_ERR_NO_ERROR) return 1;

   p = (char *) gpgme_get_protocol_name(GPGME_PROTOCOL_OpenPGP);
   printf("Protocol name: %s\n",p);

   /* get engine information */
   err = gpgme_get_engine_info(&enginfo);
   if(err != GPG_ERR_NO_ERROR) return 2;
   printf("file=%s, home=%s\n",enginfo->file_name,enginfo->home_dir);

   /* create our own context */
   err = gpgme_new(ctx);
   if(err != GPG_ERR_NO_ERROR) return 3;

   /* set protocol to use in our context */
   err = gpgme_set_protocol(*ctx,GPGME_PROTOCOL_OpenPGP);
   if(err != GPG_ERR_NO_ERROR) return 4;

   /* set engine info in our context; I changed it for ceof like this:

   err = gpgme_ctx_set_engine_info (*ctx, GPGME_PROTOCOL_OpenPGP,
               "/usr/bin/gpg","/home/user/nico/.ceof/gpg/");

      but I'll use standard values for this example: */

   err = gpgme_ctx_set_engine_info (*ctx, GPGME_PROTOCOL_OpenPGP,
               enginfo->file_name,enginfo->home_dir);
   if(err != GPG_ERR_NO_ERROR) return 5;

   return 0;
}

void print_gpg_error(gpgme_error_t err)
{
   printf("%s\n", gpg_strerror(err));
}

int main(void)
{
   gpgme_ctx_t ctx;
   gpgme_error_t err;

   int setup_res = setup(&ctx);
   if(setup_res) return setup_res;

   /* Change mode to list sigs and */
   gpgme_keylist_mode_t mode = gpgme_get_keylist_mode(ctx);
   mode |= GPGME_KEYLIST_MODE_SIGS | GPGME_KEYLIST_MODE_SIG_NOTATIONS;
   err = gpgme_set_keylist_mode(ctx, mode);
   if (err != GPG_ERR_NO_ERROR) return 6;

   /* List all keys */
   /* (context, pattern, secret_only) */
   err = gpgme_op_keylist_start(ctx, NULL, 0);
   if (err != GPG_ERR_NO_ERROR) return 7;

   gpgme_key_t key;
   while ((err = gpgme_op_keylist_next(ctx, &key)) == GPG_ERR_NO_ERROR) {
      printf("Got a key\n");
      printf ("%s:", key->subkeys->keyid);
      if (key->uids && key->uids->name)
        printf (" %s", key->uids->name);
      if (key->uids && key->uids->email)
        printf (" <%s>", key->uids->email);
      if (key->uids && key->uids->signatures) {
        printf("First signature: %s\n", key->uids->signatures->keyid);
      }
      putchar ('\n');
      gpgme_key_release (key);
   }
   if (gpg_err_code(err) == GPG_ERR_EOF) {
      printf("%xu\n", err);
      printf("End of keylist.\n");
   } else {
      print_gpg_error(err);
      return 8;
   }

   /* free context */
   gpgme_release(ctx);

   return 0;
}
