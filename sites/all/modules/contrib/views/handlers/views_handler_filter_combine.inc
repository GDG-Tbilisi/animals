<?php

/**
 * @file
 * Definition of views_handler_filter_combine.
 */

/**
 * Filter handler which allows to animals_search_block on multiple fields.
 *
 * @ingroup views_field_handlers
 */
class views_handler_filter_combine extends views_handler_filter_string {
  /**
   * @var views_plugin_query_default
   */
  public $query;

  function option_definition() {
    $options = parent::option_definition();
    $options['fields'] = array('default' => array());

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $this->view->init_style();

    // Allow to choose all fields as possible.
    if ($this->view->style_plugin->uses_fields()) {
      $options = array();
      foreach ($this->view->display_handler->get_handlers('field') as $name => $field) {
        $options[$name] = $field->ui_name(TRUE);
      }
      if ($options) {
        $form['fields'] = array(
          '#type' => 'select',
          '#title' => t('Choose fields to combine for filtering'),
          '#description' => t("This filter doesn't work for very special field handlers."),
          '#multiple' => TRUE,
          '#options' => $options,
          '#default_value' => $this->options['fields'],
        );
      }
      else {
        form_set_error('', t('You have to add some fields to be able to use this filter.'));
      }
    }
  }

  function query() {
    $this->view->_build('field');
    $fields = array();
    // Only add the fields if they have a proper field and table alias.
    foreach ($this->options['fields'] as $id) {
      $field = $this->view->field[$id];
      // Always add the table of the selected fields to be sure a table alias
      // exists.
      $field->ensure_my_table();
      if (!empty($field->field_alias) && !empty($field->field_alias)) {
        $fields[] = "$field->table_alias.$field->real_field";
      }
    }
    if ($fields) {
      $count = count($fields);
      $separated_fields = array();
      foreach ($fields as $key => $field) {
        $separated_fields[] = $field;
        if ($key < $count - 1) {
          $separated_fields[] = "' '";
        }
      }
      $expression = implode(', ', $separated_fields);
      $expression = "CONCAT_WS(' ', $expression)";

      $info = $this->operators();
      if (!empty($info[$this->operator]['method'])) {
        $this->{$info[$this->operator]['method']}($expression);
      }
    }
  }

  // By default things like op_equal uses add_where, that doesn't support
  // complex expressions, so override all operators.
  function op_equal($field) {
    $placeholder = $this->placeholder();
    $operator = $this->operator();
    $this->query->add_where_expression($this->options['group'], "$field $operator $placeholder", array($placeholder => $this->value));
  }

  function op_contains($field) {
    $placeholder = $this->placeholder();
    $this->query->add_where_expression($this->options['group'], "$field LIKE $placeholder", array($placeholder => '%' . db_like($this->value) . '%'));
  }

  function op_word($field) {
    $where = $this->operator == 'word' ? db_or() : db_and();

    // Don't filter on empty strings.
    if (empty($this->value)) {
      return;
    }

    preg_match_all('/ (-?)("[^"]+"|[^" ]+)/i', ' ' . $this->value, $matches, PREG_SET_ORDER);
    foreach ($matches as $match) {
      $phrase = FALSE;
      // Strip off phrase quotes.
      if ($match[2]{0} == '"') {
        $match[2] = substr($match[2], 1, -1);
        $phrase = TRUE;
      }
      $words = trim($match[2], ',?!();:-');
      $words = $phrase ? array($words) : preg_split('/ /', $words, -1, PREG_SPLIT_NO_EMPTY);
      $placeholder = $this->placeholder();
      foreach ($words as $word) {
        $where->where($field . " LIKE $placeholder", array($placeholder => '%' . db_like(trim($word, " ,!?")) . '%'));
      }
    }

    if (!$where) {
      return;
    }

    // Previously this was a call_user_func_array() but that's unnecessary
    // as views will unpack an array that is a single arg.
    $this->query->add_where($this->options['group'], $where);
  }

  function op_starts($field) {
    $placeholder = $this->placeholder();
    $this->query->add_where_expression($this->options['group'], "$field LIKE $placeholder", array($placeholder => db_like($this->value) . '%'));
  }

  function op_not_starts($field) {
    $placeholder = $this->placeholder();
    $this->query->add_where_expression($this->options['group'], "$field NOT LIKE $placeholder", array($placeholder => db_like($this->value) . '%'));
  }

  function op_ends($field) {
    $placeholder = $this->placeholder();
    $this->query->add_where_expression($this->options['group'], "$field LIKE $placeholder", array($placeholder => '%' . db_like($this->value)));
  }

  function op_not_ends($field) {
    $placeholder = $this->placeholder();
    $this->query->add_where_expression($this->options['group'], "$field NOT LIKE $placeholder", array($placeholder => '%' . db_like($this->value)));
  }

  function op_not($field) {
    $placeholder = $this->placeholder();
    $this->query->add_where_expression($this->options['group'], "$field NOT LIKE $placeholder", array($placeholder => '%' . db_like($this->value) . '%'));
  }

  function op_regex($field) {
    $placeholder = $this->placeholder();
    $this->query->add_where_expression($this->options['group'], "$field RLIKE $placeholder", array($placeholder => $this->value));
  }

  function op_empty($field) {
    if ($this->operator == 'empty') {
      $operator = "IS NULL";
    }
    else {
      $operator = "IS NOT NULL";
    }

    $this->query->add_where_expression($this->options['group'], "$field $operator");
  }
}
