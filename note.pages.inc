<?php

/**
 * @file
 * Page callback file for the note module.
 */

/**
 * Menu callback; displays a Drupal page containing recent note entries of a given user.
 */
function note_page_user($account) {
  global $user;

  drupal_set_title($title = t("@name's note", array('@name' => format_username($account))), PASS_THROUGH);

  $build = array();

  $query = db_select('node', 'n')->extend('PagerDefault');
  $nids = $query
    ->fields('n', array('nid', 'sticky', 'created'))
    ->condition('type', 'note')
    ->condition('uid', $account->uid)
    ->condition('status', 1)
    ->orderBy('sticky', 'DESC')
    ->orderBy('created', 'DESC')
    ->limit(variable_get('default_nodes_main', 10))
    ->addTag('node_access')
    ->execute()
    ->fetchCol();

  if (!empty($nids)) {
    $nodes = node_load_multiple($nids);
    $build += node_view_multiple($nodes);
    $build['pager'] = array(
      '#theme' => 'pager',
      '#weight' => 5,
    );
  }
  else {
    if ($account->uid == $user->uid) {
      drupal_set_message(t('You have not created any note entries.'));
    }
    else {
      drupal_set_message(t('!author has not created any note entries.', array('!author' => theme('username', array('account' => $account)))));
    }
  }
  drupal_add_feed('note/' . $account->uid . '/feed', t('RSS - !title', array('!title' => $title)));

  return $build;
}