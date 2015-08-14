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

// From GPGME tests
static const char *
nonnull (const char *s)
{
  return s? s :"[none]";
}


void print_import_result (gpgme_import_result_t r)
{
  gpgme_import_status_t st;

  for (st=r->imports; st; st = st->next)
    {
      printf ("  fpr: %s err: %d (%s) status:", nonnull (st->fpr),
              st->result, gpgme_strerror (st->result));
      if (st->status & GPGME_IMPORT_NEW)
        fputs (" new", stdout);
      if (st->status & GPGME_IMPORT_UID)
        fputs (" uid", stdout);
      if (st->status & GPGME_IMPORT_SIG)
        fputs (" sig", stdout);
      if (st->status & GPGME_IMPORT_SUBKEY)
        fputs (" subkey", stdout);
      if (st->status & GPGME_IMPORT_SECRET)
        fputs (" secret", stdout);
      putchar ('\n');
    }
  printf ("key import summary:\n"
          "        considered: %d\n"
          "        no user id: %d\n"
          "          imported: %d\n"
          "      imported_rsa: %d\n"
          "         unchanged: %d\n"
          "      new user ids: %d\n"
          "       new subkeys: %d\n"
          "    new signatures: %d\n"
          "   new revocations: %d\n"
          "       secret read: %d\n"
          "   secret imported: %d\n"
          "  secret unchanged: %d\n"
          "  skipped new keys: %d\n"
          "      not imported: %d\n",
          r->considered,
          r->no_user_id,
          r->imported,
          r->imported_rsa,
          r->unchanged,
          r->new_user_ids,
          r->new_sub_keys,
          r->new_signatures,
          r->new_revocations,
          r->secret_read,
          r->secret_imported,
          r->secret_unchanged,
          r->skipped_new_keys,
          r->not_imported);
}


int main(void)
{
   gpgme_ctx_t ctx;
   gpgme_error_t err;

   int setup_res = setup(&ctx);
   if(setup_res) return setup_res;

   /* Change mode to extern */
   gpgme_keylist_mode_t mode = gpgme_get_keylist_mode(ctx);
   mode &= ~GPGME_KEYLIST_MODE_LOCAL;
   mode |= GPGME_KEYLIST_MODE_EXTERN;
   mode |= GPGME_KEYLIST_MODE_SIGS | GPGME_KEYLIST_MODE_SIG_NOTATIONS;
   err = gpgme_set_keylist_mode(ctx, mode);
   if (err != GPG_ERR_NO_ERROR) return 6;

   /* List all keys */
   /* (context, pattern, secret_only) */
   err = gpgme_op_keylist_start(ctx, "07E7871B", 0);
   if (err != GPG_ERR_NO_ERROR) return 7;

   gpgme_key_t key;
   size_t MAX_KEYS = 100;
   gpgme_key_t keyarray[MAX_KEYS+1]; // need a space for the NULL term
   size_t i = 0;

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
      if (i < MAX_KEYS) {
        keyarray[i++] = key;
      } else {
        printf("Too many keys, skipping a key\n");
      }
      //gpgme_key_release (key);
   }
   if (gpg_err_code(err) == GPG_ERR_EOF) {
      printf("%xu\n", err);
      printf("End of keylist.\n");
   } else {
      print_gpg_error(err);
      return 8;
   }

   printf("Attempting to import %lu keys\n", i);
   err = gpgme_op_import_keys(ctx, keyarray);
   if (err != GPG_ERR_NO_ERROR) return 9;
   gpgme_import_result_t impres = gpgme_op_import_result(ctx);
   if (!impres) return 10;
   print_import_result(impres);

   /* free keys */
   for (i = 0; keyarray[i]; i++)
     gpgme_key_release(key);

   /* free context */
   gpgme_release(ctx);

   return 0;
}
