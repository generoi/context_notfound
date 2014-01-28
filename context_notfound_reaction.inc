<?php
/**
 * @file
 * Context not found reaction plugin for Context API.
 */

class context_notfound_reaction extends context_reaction {

  private $delivered = FALSE;

  /**
   * Implements options_form().
   */
  public function options_form($context) {
    $values = $this->fetch_from_context($context);
    $form['#tree'] = TRUE;
    $form['deliver_type'] = array(
      '#title' => t('Delivery type'),
      '#type' => 'select',
      '#default_value' => isset($values['deliver_type']) ? $values['deliver_type'] : 'drupal_not_found',
      '#options' => array(
        'drupal_not_found' => 'Not found',
        'drupal_access_denied' => 'Access denied',
      ),
    );

    $roles = user_roles();
    $form['admin_roles'] = array(
      '#title' => t('Allow access for these roles'),
      '#type' => 'select',
      '#default_value' => isset($values['admin_roles']) ? $values['admin_roles'] : DRUPAL_AUTHENTICATED_RID,
      '#description' => t('You can optionally allow users of these roles to access these pages, they will be notified that they are blocked for regular users.'),
      '#options' => $roles,
      '#multiple' => TRUE,
    );

    return $form;
  }

  /**
   * Implements execute().
   */
  public function execute() {
    $contexts = $this->get_contexts();

    // Check if we need to react to current Contexts.
    foreach ($contexts as $context) {
      if (!empty($context->reactions[$this->plugin])) {
        if ($function = $context->reactions[$this->plugin]['deliver_type']) {
          if ($admin_roles = $context->reactions[$this->plugin]['admin_roles']) {
            global $user;
            // If the user has a role in common with the admin roles configured 
            // deliver the page as usual but display a message informing them 
            // this page is not accessible.
            $common_roles = array_intersect(array_keys($admin_roles), array_keys($user->roles));
            if (!empty($common_roles)) {
              drupal_set_message(t('This page is blocked from regular users by a context.'));
              return;
            }
          }

          // Deliver
          if (!$this->delivered) {
            $this->delivered = TRUE;
            $function();
            drupal_exit();
          }
        }
      }
    }
  }
}
