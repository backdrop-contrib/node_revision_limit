<?php
/**
 * @file
 * Admin overview page for Node Revision Limit.
 */

/**
 * Admin overview form.
 *
 * Shows all content types with their current revision limit and node count,
 * and provides a button to queue a bulk prune pass for all limited types.
 */
function node_revision_limit_admin_form($form, &$form_state) {
  $types = node_type_get_types();
  $queue = BackdropQueue::get('node_revision_limit_prune');
  $queue_count = $queue->numberOfItems();

  $draft_workflow_active = module_exists('draft_workflow');

  $form['intro'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . t('Per-type revision limits are configured on each <a href="!url">content type edit page</a>, in the Revision settings section.', array('!url' => url('admin/structure/types'))) . '</p>',
  );

  $form['queue_status'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . t('Nodes currently in the prune queue: <strong>@count</strong>. These will be processed during the next cron run.', array('@count' => $queue_count)) . '</p>',
  );

  if ($draft_workflow_active) {
    $form['draft_workflow_note'] = array(
      '#type' => 'markup',
      '#markup' => '<p><em>' . t('Draft Workflow is active. Forward draft revisions (newer than the current revision) are never counted toward limits or removed by this module.') . '</em></p>',
    );
  }

  $rows = array();
  foreach ($types as $type_name => $type) {
    $limit = config_get('node.type.' . $type_name, 'settings.node_revision_limit');
    $limit_display = ($limit === NULL || $limit === '') ? t('Unlimited') : (int) $limit;

    $node_count = db_query(
      'SELECT COUNT(*) FROM {node} WHERE type = :type',
      array(':type' => $type_name)
    )->fetchField();

    $rows[] = array(
      check_plain($type->name),
      $limit_display,
      number_format($node_count),
      l(t('Edit'), 'admin/structure/types/manage/' . $type_name),
    );
  }

  $form['types_table'] = array(
    '#theme' => 'table',
    '#header' => array(
      t('Content type'),
      t('Previous revision limit'),
      t('Node count'),
      t('Actions'),
    ),
    '#rows' => $rows,
    '#empty' => t('No content types found.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['queue_all'] = array(
    '#type' => 'submit',
    '#value' => t('Queue prune for all limited types now'),
    '#submit' => array('node_revision_limit_admin_queue_all_submit'),
  );

  return $form;
}

/**
 * Submit handler: queues a prune pass for every content type that has a limit set.
 */
function node_revision_limit_admin_queue_all_submit($form, &$form_state) {
  $types = node_type_get_types();
  $total_nodes = 0;
  $type_count = 0;

  foreach ($types as $type_name => $type) {
    $limit = config_get('node.type.' . $type_name, 'settings.node_revision_limit');
    if ($limit !== NULL && $limit !== '') {
      $queued = node_revision_limit_queue_type($type_name, (int) $limit);
      $total_nodes += $queued;
      $type_count++;
    }
  }

  if ($type_count) {
    backdrop_set_message(t(
      'Queued @nodes node(s) across @types content type(s) for revision pruning. Run cron to process.',
      array('@nodes' => $total_nodes, '@types' => $type_count)
    ));
  }
  else {
    backdrop_set_message(t('No content types have a revision limit configured. Set limits on the content type edit pages.'), 'warning');
  }
}
