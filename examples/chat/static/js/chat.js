$(function() {

    // Find the list of messages
    var $messages = $('.messages');

    // Find the message input box and set the appropriate handler for [enter]
    var $input = $('#input').focus().keypress(function(e) {
        if(e.which == 13) {

            // Create the new message to append
            $('<div>')
                .addClass('message')
                .text($(this).val())
                .appendTo($messages);

            // Clear the contents
            $(this).val('');
            return false;
        }
    });
});
