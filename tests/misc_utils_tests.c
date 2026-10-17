/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cJSON_Utils.h"
#include "common.h"
#include "unity/src/unity.h"

static void
cjson_utils_functions_shouldnt_crash_with_null_pointers (void)
{
  cJSON *item = cJSON_CreateString ("item");
  TEST_ASSERT_NOT_NULL (item);

  TEST_ASSERT_NULL (cJSONUtils_GetPointer (item, NULL));
  TEST_ASSERT_NULL (cJSONUtils_GetPointer (NULL, "pointer"));
  TEST_ASSERT_NULL (cJSONUtils_GetPointerCaseSensitive (NULL, "pointer"));
  TEST_ASSERT_NULL (cJSONUtils_GetPointerCaseSensitive (item, NULL));
  TEST_ASSERT_NULL (cJSONUtils_GeneratePatches (item, NULL));
  TEST_ASSERT_NULL (cJSONUtils_GeneratePatches (NULL, item));
  TEST_ASSERT_NULL (cJSONUtils_GeneratePatchesCaseSensitive (item, NULL));
  TEST_ASSERT_NULL (cJSONUtils_GeneratePatchesCaseSensitive (NULL, item));
  cJSONUtils_AddPatchToArray (item, "path", "add", NULL);
  cJSONUtils_AddPatchToArray (item, "path", NULL, item);
  cJSONUtils_AddPatchToArray (item, NULL, "add", item);
  cJSONUtils_AddPatchToArray (NULL, "path", "add", item);
  cJSONUtils_ApplyPatches (item, NULL);
  cJSONUtils_ApplyPatches (NULL, item);
  cJSONUtils_ApplyPatchesCaseSensitive (item, NULL);
  cJSONUtils_ApplyPatchesCaseSensitive (NULL, item);
  TEST_ASSERT_NULL (cJSONUtils_MergePatch (item, NULL));
  item = cJSON_CreateString ("item");
  TEST_ASSERT_NULL (cJSONUtils_MergePatchCaseSensitive (item, NULL));
  item = cJSON_CreateString ("item");
  /* these calls are actually valid */
  /* cJSONUtils_MergePatch(NULL, item); */
  /* cJSONUtils_MergePatchCaseSensitive(NULL, item);*/
  /* cJSONUtils_GenerateMergePatch(item, NULL); */
  /* cJSONUtils_GenerateMergePatch(NULL, item); */
  /* cJSONUtils_GenerateMergePatchCaseSensitive(item, NULL); */
  /* cJSONUtils_GenerateMergePatchCaseSensitive(NULL, item); */

  TEST_ASSERT_NULL (cJSONUtils_FindPointerFromObjectTo (item, NULL));
  TEST_ASSERT_NULL (cJSONUtils_FindPointerFromObjectTo (NULL, item));
  cJSONUtils_SortObject (NULL);
  cJSONUtils_SortObjectCaseSensitive (NULL);

  cJSON_Delete (item);
}

static void
sort_object_should_restore_prev_tail_invariant (void)
{
  /* cJSON relies on child->prev pointing to the list tail (used by
   * add_item_to_array() for O(1) appends). The mergesort in sort_list()
   * must restore that invariant, otherwise a later delete+add performs a
   * use-after-free write into the stale prev node (issue #1090). */
  cJSON *object = cJSON_Parse ("{\"a\":1,\"b\":1,\"c\":1,\"e\":1,\"d\":1}");
  cJSON *tail = NULL;
  TEST_ASSERT_NOT_NULL (object);

  cJSONUtils_SortObject (object);

  tail = object->child;
  while ((tail != NULL) && (tail->next != NULL))
    {
      tail = tail->next;
    }
  TEST_ASSERT_NOT_NULL (tail);
  TEST_ASSERT_EQUAL_PTR (tail, object->child->prev);

  /* delete the interior node head->prev used to point at, then append */
  cJSON_DeleteItemFromObject (object, "d");
  cJSON_AddItemToObject (object, "z", cJSON_CreateNumber (1));

  tail = object->child;
  while ((tail != NULL) && (tail->next != NULL))
    {
      tail = tail->next;
    }
  TEST_ASSERT_NOT_NULL (tail);
  TEST_ASSERT_EQUAL_STRING ("z", tail->string);
  TEST_ASSERT_EQUAL_PTR (tail, object->child->prev);

  cJSON_Delete (object);
}

int
main (void)
{
  UNITY_BEGIN ();

  RUN_TEST (cjson_utils_functions_shouldnt_crash_with_null_pointers);
  RUN_TEST (sort_object_should_restore_prev_tail_invariant);

  return UNITY_END ();
}
