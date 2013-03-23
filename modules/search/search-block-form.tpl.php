<?php

/**
 * @file
 * Displays the animals_search_block form block.
 *
 * Available variables:
 * - $search_form: The complete animals_search_block form ready for print.
 * - $animals_search_block: Associative array of animals_search_block elements. Can be used to print each
 *   form element separately.
 *
 * Default elements within $animals_search_block:
 * - $animals_search_block['search_block_form']: Text input area wrapped in a div.
 * - $animals_search_block['actions']: Rendered form buttons.
 * - $animals_search_block['hidden']: Hidden form elements. Used to validate forms when
 *   submitted.
 *
 * Modules can add to the animals_search_block form, so it is recommended to check for their
 * existence before printing. The default keys will always exist. To check for
 * a module-provided field, use code like this:
 * @code
 *   <?php if (isset($animals_search_block['extra_field'])): ?>
 *     <div class="extra-field">
 *       <?php print $animals_search_block['extra_field']; ?>
 *     </div>
 *   <?php endif; ?>
 * @endcode
 *
 * @see template_preprocess_search_block_form()
 */
?>
<div class="container-inline">
  <?php if (empty($variables['form']['#block']->subject)): ?>
    <h2 class="element-invisible"><?php print t('Search form'); ?></h2>
  <?php endif; ?>
  <?php print $search_form; ?>
</div>
